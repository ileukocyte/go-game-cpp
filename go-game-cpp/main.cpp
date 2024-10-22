#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

enum class Sign : char {
    CROSS = 'x',
    NOUGHT = 'o',
};

class Board {
public:
    Board(size_t size) : size(size), board(size, std::vector<char>(size, '.')) {
        if (size == 0) {
            throw std::out_of_range("The size of the board cannot equal to 0!");
        }
    }

    const std::vector<std::vector<char>>& get_board_vec() const noexcept {
        return board;
    }

    void print_board(bool enable_indices = true) const noexcept {
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

    void set_cell(size_t x, size_t y, Sign player) {
        // TODO: game rules
        board.at(x).at(y) = static_cast<char>(player);
    }
private:
    const size_t size;
    std::vector<std::vector<char>> board;
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Invalid argument count!\n";

        return 1;
    }

    size_t board_size{};

    try {
        auto board_size_ul = std::stoull(argv[1]);

        if (board_size_ul > std::numeric_limits<size_t>::max()) {
            throw std::out_of_range("The value is too large for size_t");
        }

        board_size = static_cast<size_t>(board_size_ul);
    } catch (const std::invalid_argument& err) {
        std::cerr << "Invalid argument: " << err.what() << '\n';

        return 1;
    } catch (const std::out_of_range& err) {
        std::cerr << "Out of range: " << err.what() << '\n';

        return 1;
    }

    auto board_option = std::string(argv[2]);

    std::transform(board_option.begin(), board_option.end(), board_option.begin(), ::tolower);

    if (board_option != "--board" && board_option != "--score") {
        std::cerr << "The second argument must be either \"--board\" or \"--score\"!\n";

        return 1;
    }

    try {
        Board board(board_size);

        //board.set_cell(1, 1, Sign::CROSS);
        //board.set_cell(1, 2, Sign::NOUGHT);
        board.print_board();

        auto is_over = false;
        auto is_x = true;

        //while (!is_over) {

        //}
    } catch (const std::out_of_range& err) {
        std::cerr << "Out of range: " << err.what() << '\n';

        return 1;
    }

    return 0;
}
