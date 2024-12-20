#include <iomanip>
#include <iostream>
#include <optional>
#include <random>

#include "Board.h"

Board::Board(size_t size) :
    size_(size),
    board_(size, std::vector<char>(size, '.')),
    x_points_(0),
    o_points_(0),
    zobrist_table_(size, std::vector<std::vector<uint64_t>>(size, std::vector<uint64_t>(3)))
{
    if (size == 0) {
        throw std::out_of_range("The size of the board cannot equal to 0!");
    }

    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

    for (auto i = 0; i < size; ++i) {
        for (auto j = 0; j < size; ++j) {
            for (auto k = 0; k < 3; ++k) {
                zobrist_table_[i][j][k] = dis(gen);
            }
        }
    }
}

std::ostream& operator<<(std::ostream& out, const Board& board) noexcept {
    auto max_digit_count = static_cast<size_t>(floor(log10(board.size_))) + 1;

    out << std::string(max_digit_count + 1, ' ');

    for (size_t i = 0; i < board.size_; i++) {
        out << std::setfill(' ') << std::setw(max_digit_count) << i << ' ';
    }

    out << '\n';

    auto i = 0;

    for (const auto& row : board.board_) {
        if (i > 0) {
            out << '\n';
        }

        out << std::setfill(' ') << std::setw(max_digit_count) << i++;

        for (const auto& cell : row) {
            out << std::setfill(' ') << std::setw(max_digit_count + 1) << cell;
        }
    }

    return out;
}

void Board::occupy_cell(size_t x, size_t y, Turn current_turn) {
    auto cell = &board_.at(x).at(y);

    if (*cell != '.') {
        throw std::invalid_argument("The cell is already occupied!");
    }

    *cell = static_cast<char>(current_turn);

    auto probable_suicide = !has_liberties(x, y);

    auto opp_char = static_cast<char>(get_opp_turn(current_turn));

    std::vector<std::vector<char>> copy(board_);

    for (size_t i = 0; i < size_; i++) {
        for (size_t j = 0; j < size_; j++) {
            if (board_[i][j] == opp_char) {
                if (!has_liberties(i, j)) {
                    probable_suicide = false;

                    copy[i][j] = '.';

                    if (current_turn == Turn::CROSS) {
                        x_points_++;
                    } else {
                        o_points_++;
                    }
                }
            }
        }
    }

    if (probable_suicide) {
        *cell = '.';

        throw std::invalid_argument("This move would result in an unprofitable suicide!");
    }

    auto hash = calculate_hash();

    if (state_map_.find(hash) != state_map_.end()) {
        *cell = '.';

        throw std::invalid_argument("This move is forbidden by the ko rule!");
    }

    state_map_[hash] = true;
    board_ = copy;
}

bool Board::liberty_check(
    size_t i,
    size_t j,
    char sign,
    std::vector<std::vector<bool>>& visited
) noexcept {
    if (i >= size_ || j >= size_ || visited[i][j]) {
        return false;
    }

    if (board_[i][j] == '.') {
        return true;
    }

    if (board_[i][j] != sign) {
        return false;
    }

    visited[i][j] = true;

    auto has_liberty = (i > 0 && liberty_check(i - 1, j, sign, visited)) ||
        (j > 0 && liberty_check(i, j - 1, sign, visited)) ||
        (i < size_ - 1 && liberty_check(i + 1, j, sign, visited)) ||
        (j < size_ - 1 && liberty_check(i, j + 1, sign, visited));

    return has_liberty;
}

bool Board::has_liberties(size_t i, size_t j) noexcept {
    auto cell = board_[i][j];

    std::vector<std::vector<bool>> visited(size_, std::vector<bool>(size_, false));

    return liberty_check(i, j, cell, visited);
}

void Board::fill_blank_region(
    size_t i,
    size_t j,
    std::vector<std::vector<bool>>& visited,
    std::vector<std::pair<size_t, size_t>>& region
) noexcept {
    if (i < 0 || j < 0 || i >= size_ || j >= size_) {
        return;
    }

    if (visited[i][j] || board_[i][j] != '.') {
        return;
    }

    region.push_back(std::make_pair(i, j));
    visited[i][j] = true;

    fill_blank_region(i - 1, j, visited, region);
    fill_blank_region(i + 1, j, visited, region);
    fill_blank_region(i, j - 1, visited, region);
    fill_blank_region(i, j + 1, visited, region);
}

std::pair<unsigned, unsigned> Board::count_territories() noexcept {
    auto territory_owner = [&](const std::vector<std::pair<size_t, size_t>>& region) -> std::optional<char> {
        auto owner_consistent = std::make_pair('\0', true);

        for (const auto& [x, y] : region) {
            if (x > 0 && owner_consistent.second) {
                if (board_[x - 1][y] != '.') {
                    if (owner_consistent.first == '\0') {
                        owner_consistent.first = board_[x - 1][y];
                    } else {
                        owner_consistent.second = owner_consistent.first == board_[x - 1][y];
                    }
                }
            }

            if (y > 0 && owner_consistent.second) {
                if (board_[x][y - 1] != '.') {
                    if (owner_consistent.first == '\0') {
                        owner_consistent.first = board_[x][y - 1];
                    } else {
                        owner_consistent.second = owner_consistent.first == board_[x][y - 1];
                    }
                }
            }

            if (x < size_ - 1 && owner_consistent.second) {
                if (board_[x + 1][y] != '.') {
                    if (owner_consistent.first == '\0') {
                        owner_consistent.first = board_[x + 1][y];
                    } else {
                        owner_consistent.second = owner_consistent.first == board_[x + 1][y];
                    }
                }
            }

            if (y < size_ - 1 && owner_consistent.second) {
                if (board_[x][y + 1] != '.') {
                    if (owner_consistent.first == '\0') {
                        owner_consistent.first = board_[x][y + 1];
                    } else {
                        owner_consistent.second = owner_consistent.first == board_[x][y + 1];
                    }
                }
            }
        }

        return owner_consistent.second ? std::make_optional(owner_consistent.first) : std::nullopt;
    };

    unsigned x_territory = 0, o_territory = 0;

    std::vector<std::vector<bool>> visited(size_, std::vector<bool>(size_, false));

    for (size_t i = 0; i < size_; i++) {
        for (size_t j = 0; j < size_; j++) {
            if (board_[i][j] == '.' && !visited[i][j]) {
                std::vector<std::pair<size_t, size_t>> region{};

                fill_blank_region(i, j, visited, region);

                if (auto owner = territory_owner(region)) {
                    switch (static_cast<Turn>(owner.value())) {
                    case Turn::CROSS:
                        x_territory += static_cast<unsigned>(region.size());

                        break;
                    case Turn::NOUGHT:
                        o_territory += static_cast<unsigned>(region.size());

                        break;
                    }
                }
            }
        }
    }

    return std::make_pair(x_territory, o_territory);
}

uint64_t Board::calculate_hash() const noexcept {
    uint64_t hash{};

    for (auto i = 0; i < size_; i++) {
        for (auto j = 0; j < size_; j++) {
            auto cell = board_[i][j];
            auto piece = cell == '.' ? 0 : (cell == static_cast<char>(Turn::CROSS) ? 1 : 2);

            hash ^= zobrist_table_[i][j][piece];
        }
    }

    return hash;
}
