#include <cstdint>
#include <expected>
#include <functional>
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
    ss << "hello ";

    auto v = (acc::match_("hello") >> acc::map_(acc::match_(' '), [](char c) { return "SPACE"; }))(ss);

    if (v) {
        std::apply([](auto&&... args) {
            ((std::cout << args << "\n"), ...);
        },
                   v.value());
    } else {
        std::cout << v.error() << std::endl;
    };

    return EXIT_SUCCESS;
}