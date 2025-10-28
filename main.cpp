#include <array>
#include <bitset>
#include <cstdint>
#include <expected>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <variant>

#include "accx86_64/cli/acccli.hpp"
#include "accx86_64/frontend/acclexer.hpp"
#include "accx86_64/utils/acclog.hpp"

// int x : mut = 4;
int f(int x, int y) {
    x = 4;
    return x;
};

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
   
    return EXIT_SUCCESS;
}