#include <cstdint>
#include <iostream>
#include <sstream>

#include "acc/include/acccli.hpp"
#include "acc/include/acclog.hpp"

auto main(int argc, char** argv) -> int {
    if (argc <= 1) {
        acc::logger::instance().send(acc::logger::LEVEL::FATAL, "no input files determined for Ace compilation.");
        return EXIT_FAILURE;
    } else {
        std::stringstream ss;
        for (int i = 1; i < argc; i++) {
            ss << std::string(argv[i]) << " ";
        }

        acc::cli acli(std::move(ss.str()));
    };

    return EXIT_SUCCESS;
}