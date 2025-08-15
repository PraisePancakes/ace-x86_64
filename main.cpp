#include <cstdint>
#include <iostream>
#include <sstream>
#include <variant>

#include "acc/include/acccli.hpp"
#include "acc/include/acclog.hpp"

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
    ss << "matchmatchmatch";
    auto matcher = acc::match_("match");
    auto v = matcher(ss);

    std::cout << v.value_or("error");

    return EXIT_SUCCESS;
}