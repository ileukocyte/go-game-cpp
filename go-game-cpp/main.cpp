#include <algorithm>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        return 1;
    }

    unsigned board_size{};

    try {
        auto board_size_ul = std::stoul(argv[1]);

        if (board_size_ul > std::numeric_limits<unsigned>::max()) {
            throw std::out_of_range("");
        }

        board_size = static_cast<unsigned>(board_size_ul);
    }
    catch (const std::invalid_argument& err) {
        return 1;
    }
    catch (const std::out_of_range& err) {
        return 1;
    }

    auto board_option = std::string(argv[2]);

    std::transform(board_option.begin(), board_option.end(), board_option.begin(), ::tolower);

    if (board_option != "--board" && board_option != "--score") {
        return 1;
    }

    return 0;
}
