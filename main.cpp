#include <cstdint>
#include <expected>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <variant>

#include "accx86_64/cli/acccli.hpp"
#include "accx86_64/frontend/accparser.hpp"
#include "accx86_64/utils/acclog.hpp"
#include "test/test_frontend/test_lexer.hpp"
#include "test/test_frontend/test_parser.hpp"
#include "test/test_utils/test_eval.hpp"

auto main(int argc, char** argv) -> int {
    // if (argc <= 1) {
    //     acc::logger::instance().send(acc::logger::LEVEL::FATAL, "no input files determined for Ace compilation.", std::cout);
    //     return EXIT_FAILURE;
    // }
    // std::stringstream ss;
    // for (int i = 1; i < argc; i++) {
    //     ss << std::string(argv[i]) << " ";
    // }
    // acc::cli acli(std::move(ss));

    acc::utest::TEST_LEXER();
    acc::utest::TEST_EVAL();
    acc::utest::TEST_PARSER();

    return EXIT_SUCCESS;
}