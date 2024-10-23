#include <iostream>

#include "Board.h"

Board::Board(size_t size) :
    size(size),
    board(size, std::vector<char>(size, '.')),
    x_score(0),
    o_score(0)
{
    if (size == 0) {
        throw std::out_of_range("The size of the board cannot equal to 0!");
    }
}

void Board::print_board(bool enable_indices) const noexcept {
    auto digit_count = [](size_t i) {
        return i == 0 ? 1 : static_cast<size_t>(floor(log10(i))) + 1;
    };

    if (enable_indices) {
        std::cout << std::string(digit_count(size) + 1, ' ');

        for (auto i = 0; i < size; i++) {
            std::cout << std::string(digit_count(size) - digit_count(i), ' ') << i << ' ';
        }

        std::cout << '\n';
    }

    auto y = 0;

    for (const auto& row : board) {
        auto x = 0;

        for (const auto& cell : row) {
            if (x == 0 && enable_indices) {
                std::cout << std::string(digit_count(size) - digit_count(y), ' ') << y;
            }

            std::cout << (!enable_indices ? std::string(x != 0, ' ') : std::string(digit_count(size), ' '));
            std::cout << cell;

            x++;
        }

        std::cout << '\n';

        y++;
    }
}

void Board::occupy_cell(size_t x, size_t y, Turn current_turn) {
    auto cell = &board.at(x).at(y);

    if (*cell != '.') {
        throw std::invalid_argument("The cell is already occupied!");
    }

    *cell = static_cast<char>(current_turn);

    auto probable_suicide = !has_liberties(x, y, board);

    auto opp_char = static_cast<char>(get_opp_turn(current_turn));

    auto board_copy = board;

    for (auto i = 0; i < size; i++) {
        for (auto j = 0; j < size; j++) {
            if (board[i][j] == opp_char) {
                if (!has_liberties(i, j, board)) {
                    probable_suicide = false;

                    board_copy[i][j] = '.';

                    if (current_turn == Turn::CROSS) {
                        x_score++;
                    } else {
                        o_score++;
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

    if (std::find(state_vec.begin(), state_vec.end(), state_str) != state_vec.end()) {
        *cell = '.';

        throw std::invalid_argument("This move is forbidden by the ko rule!");
    }

    board = board_copy;
    state_vec.push_back(state_str);
}

bool Board::liberty_check(
    size_t x,
    size_t y,
    std::vector<std::vector<bool>>& visited,
    char cell,
    const std::vector<std::vector<char>>& board
) {
    if (x >= size || y >= size || visited[x][y]) {
        return false;
    }

    if (board[x][y] == '.') {
        return true;
    }

    if (board[x][y] != cell) {
        return false;
    }

    visited[x][y] = true;

    auto has_liberty = false;

    if (x > 0) {
        has_liberty = has_liberty || liberty_check(x - 1, y, visited, cell, board);
    }

    if (y > 0) {
        has_liberty = has_liberty || liberty_check(x, y - 1, visited, cell, board);
    }

    if (x < size - 1) {
        has_liberty = has_liberty || liberty_check(x + 1, y, visited, cell, board);
    }

    if (y < size - 1) {
        has_liberty = has_liberty || liberty_check(x, y + 1, visited, cell, board);
    }

    return has_liberty;
}

bool Board::has_liberties(size_t x, size_t y, const std::vector<std::vector<char>>& board) {
    auto cell = board[x][y];

    std::vector<std::vector<bool>> visited(size, std::vector<bool>(size, false));

    return liberty_check(x, y, visited, cell, board);
}

Turn Board::get_opp_turn(Turn current_turn) const noexcept {
    return current_turn == Turn::CROSS ? Turn::NOUGHT : Turn::CROSS;
}

std::string Board::as_state_str() const noexcept {
    std::string state;

    for (const auto& row : board) {
        for (auto cell : row) {
            state += cell;
        }
    }

    return state;
}
