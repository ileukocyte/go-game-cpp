#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <map>
#include <vector>

enum class Turn : char {
    CROSS = 'X',
    NOUGHT = 'O',
};

class Board {
public:
    Board(std::size_t size);

    const std::vector<std::vector<char>>& get_board_vec() const noexcept {
        return board_;
    }

    unsigned get_x_points() const noexcept {
        return x_points_;
    }

    unsigned get_o_points() const noexcept {
        return o_points_;
    }

    void occupy_cell(std::size_t x, std::size_t y, Turn current_turn);

    std::pair<unsigned, unsigned> count_territories() noexcept;

    friend std::ostream& operator<<(std::ostream& out, const Board& board) noexcept;

    static Turn get_opp_turn(Turn current_turn) noexcept {
        return current_turn == Turn::CROSS ? Turn::NOUGHT : Turn::CROSS;
    }
private:
    const std::size_t size_{};
    unsigned x_points_{}, o_points_{};
    std::vector<std::vector<char>> board_{};
    std::vector<std::vector<std::vector<uint64_t>>> zobrist_table_{};
    std::map<uint64_t, bool> state_map_{};

    bool liberty_check(
        std::size_t i,
        std::size_t j,
        char sign,
        std::vector<std::vector<bool>>& visited
    ) noexcept;

    bool has_liberties(std::size_t i, std::size_t j) noexcept;

    void fill_blank_region(
        size_t i,
        size_t j,
        std::vector<std::vector<bool>>& visited,
        std::vector<std::pair<size_t, size_t>>& region
    ) noexcept;

    uint64_t calculate_hash() const noexcept;
};

#endif
