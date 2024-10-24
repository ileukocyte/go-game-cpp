#ifndef BOARD_H
#define BOARD_H

#include <unordered_set>
#include <vector>

enum class Turn : char {
    CROSS = 'X',
    NOUGHT = 'O',
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

    std::pair<unsigned, unsigned> count_territories() noexcept;

    std::string as_state_str() const noexcept;
private:
    const std::size_t size{};
    unsigned x_score{}, o_score{};
    std::vector<std::vector<char>> board{};
    std::unordered_set<std::string> state_vec{};

    bool liberty_check(
        std::size_t x,
        std::size_t y,
        char sign,
        std::vector<std::vector<bool>>& visited
    ) noexcept;

    bool has_liberties(std::size_t x, std::size_t y) noexcept;

    void fill_blank_region(
        size_t x,
        size_t y,
        std::vector<std::vector<bool>>& visited,
        std::vector<std::pair<size_t, size_t>>& region
    ) noexcept;

    Turn get_opp_turn(Turn current_turn) const noexcept;
};

#endif
