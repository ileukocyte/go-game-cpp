#include <algorithm>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

enum class Turn : char {
    CROSS = 'x',
    NOUGHT = 'o',
};

class Board {
public:
    Board(size_t size) : size(size), board(size, std::vector<char>(size, '.')), x_score(0), o_score(0) {
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

    void occupy_cell(size_t x, size_t y, Turn current_turn) {
        auto cell = &board.at(x).at(y);

        if (*cell != '.') {
            throw std::invalid_argument("The cell is already occupied!");
        }
            
        *cell = static_cast<char>(current_turn);

        auto opp_char = static_cast<char>(get_opp_turn(current_turn));

        std::vector<std::pair<size_t, size_t>> cells_to_check;

        if (x > 0) {
            cells_to_check.push_back(std::make_pair(x - 1, y));
        }

        if (y > 0) {
            cells_to_check.push_back(std::make_pair(x, y - 1));
        }

        if (x < size) {
            cells_to_check.push_back(std::make_pair(x + 1, y));
        }

        if (y < size) {
            cells_to_check.push_back(std::make_pair(x, y + 1));
        }

        for (const auto& pair : cells_to_check) {
            auto a = pair.first, b = pair.second;
            auto checked = &board[a][b];

            if (*checked == opp_char) {
                if (move_options(a, b).empty()) {
                    *checked = '.';

                    if (current_turn == Turn::CROSS) {
                        x_score++;
                    } else {
                        o_score++;
                    }
                }
            }
        }
    }
private:
    const size_t size;
    unsigned x_score, o_score;
    std::vector<std::vector<char>> board;

    std::vector<std::pair<size_t, size_t>> move_options(size_t x, size_t y) const {
        auto cell = board[x][y];

        std::vector<std::pair<size_t, size_t>> possible_moves;

        for (auto i = x; i > 0; --i) {
            if (board[i][y] != cell) {
                if (board[i][y] == '.') {
                    possible_moves.push_back(std::make_pair(i, y));
                }

                break;
            }
        }

        for (auto i = y; i > 0; --i) {
            if (board[x][i] != cell) {
                if (board[x][i] == '.') {
                    possible_moves.push_back(std::make_pair(x, i));
                }

                break;
            }
        }

        for (auto i = x; i < size; ++i) {
            if (board[i][y] != cell) {
                if (board[i][y] == '.') {
                    possible_moves.push_back(std::make_pair(i, y));
                }

                break;
            }
        }

        for (auto i = y; i < size; ++i) {
            if (board[x][i] != cell) {
                if (board[x][i] == '.') {
                    possible_moves.push_back(std::make_pair(x, i));
                }

                break;
            }
        }

        return possible_moves;
    }

    Turn get_opp_turn(Turn current_turn) const noexcept {
        return current_turn == Turn::CROSS ? Turn::NOUGHT : Turn::CROSS;
    }
};

std::optional<std::pair<size_t, size_t>> read_input(Turn current_turn) {
    size_t x{}, y{};
    std::string input;

    while (true) {
        std::cout << "Current turn: " << static_cast<char>(current_turn) << '\n';
        std::cout << "Enter two numbers (row column): ";

        std::getline(std::cin, input);
        std::istringstream iss(input);

        iss >> x >> y;

        if (iss.fail()) {
            std::transform(input.begin(), input.end(), input.begin(), ::tolower);
            
            if (input == "pass") {
                return std::nullopt;
            }

            std::cout << "Invalid input! Try again!\n";

            continue;
        }

        return std::make_pair(x, y);
    }
}

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

        board.print_board();

        auto is_over = false;
        auto is_x = true;
        auto has_passed = false;

        while (!is_over) {
            try {
                auto current_turn = is_x ? Turn::CROSS : Turn::NOUGHT;
                
                if (const auto& coords = read_input(current_turn)) {
                    board.occupy_cell(coords.value().first, coords.value().second, current_turn);

                    has_passed = false;
                } else {
                    if (has_passed) {
                        return 0;
                        // TODO: exit
                    }

                    has_passed = true;
                }

                is_x = !is_x;

                board.print_board();
            } catch (const std::out_of_range& err) {
                std::cout << "Invalid turn! Try again!\n";

                continue;
            } catch (const std::invalid_argument& err) {
                std::cout << err.what() << '\n';

                continue;
            }
        }
    } catch (const std::out_of_range& err) {
        std::cerr << "Out of range: " << err.what() << '\n';

        return 1;
    }

    return 0;
}
