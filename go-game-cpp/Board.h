#ifndef BOARD_H
#define BOARD_H

#include <vector>

enum class Turn : char {
    CROSS = 'x',
    NOUGHT = 'o',
};

class Board {
public:
    Board(std::size_t size);

    const std::vector<std::vector<char>>& get_board_vec() const noexcept {
        return board;
    }

    unsigned get_x_score() const noexcept {
        return x_score;
    }

    unsigned get_o_score() const noexcept {
        return o_score;
    }

    void print_board(bool enable_indices = true) const noexcept;

    void occupy_cell(std::size_t x, std::size_t y, Turn current_turn);
private:
    const std::size_t size{};
    unsigned x_score{}, o_score{};
    std::vector<std::vector<char>> board{};

    bool liberty_check(
        std::size_t x,
        std::size_t y,
        std::vector<std::vector<bool>>& visited,
        char cell,
        const std::vector<std::vector<char>>& board
    );

    bool has_liberties(std::size_t x, std::size_t y, const std::vector<std::vector<char>>& board);

    Turn get_opp_turn(Turn current_turn) const noexcept;
};

#endif
