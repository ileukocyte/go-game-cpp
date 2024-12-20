#include <algorithm>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "Board.h"

std::optional<std::pair<size_t, size_t>> read_input(Turn current_turn) {
    size_t x{}, y{};
    std::string input{};

    while (true) {
        std::cout << "Current turn: " << static_cast<char>(current_turn) << '\n';
        std::cout << "Enter two numbers (row column): ";

        std::getline(std::cin, input);
        std::istringstream iss(input);

        iss >> x >> y;

        if (iss.fail()) {
            std::ranges::transform(input, input.begin(), ::tolower);
            
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

    std::ranges::transform(board_option, board_option.begin(), ::tolower);

    if (board_option != "--board" && board_option != "--score") {
        std::cerr << "The second argument must be either \"--board\" or \"--score\"!\n";

        return 1;
    }

    try {
        Board board(board_size);

        std::cout << board << '\n';

        auto current_turn = Turn::CROSS;
        auto passed_last = false;

        while (true) {
            try {
                if (const auto& coords = read_input(current_turn)) {
                    board.occupy_cell(coords.value().first, coords.value().second, current_turn);

                    passed_last = false;
                } else {
                    if (passed_last) {
                        if (board_option == "--score") {
                            auto x_score = board.get_x_points(), o_score = board.get_o_points();
                            auto [x_territory, o_territory] = board.count_territories();

                            x_score += x_territory;
                            o_score += o_territory;

                            std::cout << x_score << ' ' << o_score << '\n';
                        } else {
                            std::cout << board << '\n';
                        }
                        
                        return 0;
                    }

                    passed_last = true;
                }

                current_turn = Board::get_opp_turn(current_turn);

                std::cout << board << '\n';
            } catch (const std::out_of_range&) {
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
