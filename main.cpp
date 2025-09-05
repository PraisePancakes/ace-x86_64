#include <cstdint>
#include <expected>
#include <functional>
#include <iostream>
#include <sstream>
#include <variant>

#include "acc/cli/acccli.hpp"
#include "acc/frontend/accparser.hpp"
#include "acc/utils/acclog.hpp"
#include "test/test_parser.hpp"

auto main(int argc, char** argv) -> int {
    if (argc <= 1) {
        acc::logger::instance().send(acc::logger::LEVEL::FATAL, "no input files determined for Ace compilation.", std::cout);
        return EXIT_FAILURE;
    }
    std::stringstream ss;
    for (int i = 1; i < argc; i++) {
        ss << std::string(argv[i]) << " ";
    }
    acc::cli acli(std::move(ss));
    // TEST_PARSER();

    return EXIT_SUCCESS;
}