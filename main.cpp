#include <cstdint>
#include <iostream>
#include <sstream>
#include <variant>

#include "acc/include/acclog.hpp"
#include "acc/include/cli/acccli.hpp"

auto main(int argc, char** argv) -> int {
    // if (argc <= 1) {
    //     acc::logger::instance().send(acc::logger::LEVEL::FATAL, "no input files determined for Ace compilation.");
    //     return EXIT_FAILURE;
    // }
    // std::stringstream ss;
    // for (int i = 1; i < argc; i++) {
    //     ss << std::string(argv[i]) << " ";
    // }

    // acc::cli acli(std::move(ss));

    std::stringstream ss;
    ss << "mar";
    auto matcher = acc::many_(acc::match_("match"));
    auto v = matcher(ss);
    if (v.has_value()) {
        for (auto& s : v.value()) {
            std::cout << s << std::endl;
        }
    }

    return EXIT_SUCCESS;
}