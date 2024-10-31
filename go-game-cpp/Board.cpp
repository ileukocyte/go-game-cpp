#include <iostream>
#include <optional>

#include "Board.h"

Board::Board(size_t size) :
    size_(size),
    board_(size, std::vector<char>(size, '.')),
    x_score_(0),
    o_score_(0)
{
    if (size == 0) {
        throw std::out_of_range("The size of the board cannot equal to 0!");
    }
}

void Board::print_board(bool enable_indices) const noexcept {
    auto digit_count = [](const size_t i) {
        return i == 0 ? 1 : static_cast<size_t>(floor(log10(i))) + 1;
    };

    if (enable_indices) {
        std::cout << std::string(digit_count(size_) + 1, ' ');

        for (size_t i = 0; i < size_; i++) {
            std::cout << std::string(digit_count(size_) - digit_count(i), ' ') << i << ' ';
        }

        std::cout << '\n';
    }

    auto y = 0;

    for (const auto& row : board_) {
        auto x = 0;

        for (const auto& cell : row) {
            if (x == 0 && enable_indices) {
                std::cout << std::string(digit_count(size_) - digit_count(y), ' ') << y;
            }

            std::cout << (!enable_indices ? std::string(x != 0, ' ') : std::string(digit_count(size_), ' '));
            std::cout << cell;

            x++;
        }

        std::cout << '\n';

        y++;
    }
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
                        x_score_++;
                    } else {
                        o_score_++;
                    }
                }
            }
        }
    }

    if (probable_suicide) {
        *cell = '.';

        throw std::invalid_argument("This move would result in an unprofitable suicide!");
    }

    auto state_str = as_state_str();

    if (std::ranges::find(state_vec_, state_str) != state_vec_.end()) {
        *cell = '.';

        throw std::invalid_argument("This move is forbidden by the ko rule!");
    }

    board_ = copy;
    state_vec_.insert(state_str);
}

bool Board::liberty_check(
    size_t x,
    size_t y,
    char sign,
    std::vector<std::vector<bool>>& visited
) noexcept {
    if (x >= size_ || y >= size_ || visited[x][y]) {
        return false;
    }

    if (board_[x][y] == '.') {
        return true;
    }

    if (board_[x][y] != sign) {
        return false;
    }

    visited[x][y] = true;

    auto has_liberty = false;

    if (x > 0) {
        has_liberty = has_liberty || liberty_check(x - 1, y, sign, visited);
    }

    if (y > 0) {
        has_liberty = has_liberty || liberty_check(x, y - 1, sign, visited);
    }

    if (x < size_ - 1) {
        has_liberty = has_liberty || liberty_check(x + 1, y, sign, visited);
    }

    if (y < size_ - 1) {
        has_liberty = has_liberty || liberty_check(x, y + 1, sign, visited);
    }

    return has_liberty;
}

bool Board::has_liberties(size_t x, size_t y) noexcept {
    auto cell = board_[x][y];

    std::vector<std::vector<bool>> visited(size_, std::vector<bool>(size_, false));

    return liberty_check(x, y, cell, visited);
}

void Board::fill_blank_region(
    size_t x,
    size_t y,
    std::vector<std::vector<bool>>& visited,
    std::vector<std::pair<size_t, size_t>>& region
) noexcept {
    if (x >= size_ || y >= size_) {
        return;
    }

    if (visited[x][y] || board_[x][y] != '.') {
        return;
    }

    region.push_back(std::make_pair(x, y));
    visited[x][y] = true;

    if (x > 0) {
        fill_blank_region(x - 1, y, visited, region);
    }

    if (y > 0) {
        fill_blank_region(x, y - 1, visited, region);
    }

    if (x < size_ - 1) {
        fill_blank_region(x + 1, y, visited, region);
    }

    if (y < size_ - 1) {
        fill_blank_region(x, y + 1, visited, region);
    }
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

Turn Board::get_opp_turn(Turn current_turn) const noexcept {
    return current_turn == Turn::CROSS ? Turn::NOUGHT : Turn::CROSS;
}

std::string Board::as_state_str() const noexcept {
    std::string state;

    for (const auto& row : board_) {
        for (auto cell : row) {
            state += cell;
        }
    }

    return state;
}
