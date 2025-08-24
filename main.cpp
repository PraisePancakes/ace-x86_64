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
    ss << "hellohellohello";
    auto v = acc::many_one(acc::match_("hello"))(ss);
    if (v.has_value()) {
        for (auto e : v.value().first) {
            std::cout << e << std::endl;
        }
    } else {
        std::cout << "her" << v.error() << std::endl;
    }

    return EXIT_SUCCESS;
}