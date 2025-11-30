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
#define ARGV_DEBUG false

// int x : mut = 4;
auto main(int argc, char** argv) -> int {
    char f = 3.24;
    if (argc <= 1) {
        acc::logger::instance().send(acc::logger::LEVEL::FATAL, "no input files determined for Ace compilation.", std::cout);
        return EXIT_FAILURE;
    }
    std::stringstream ss;
#if ARGV_DEBUG
    std::cout << "ARGV DEBUG START\n";
#endif
    for (int i = 1; i < argc; i++) {
#if ARGV_DEBUG

        std::cout << argv[i] << ";" << std::endl;
#endif
        ss << std::string(argv[i]) << " ";
    }
#if ARGV_DEBUG
    std::cout << "ARGV DEBUG END\n";
#endif

    try {
        acc::iface::cli acli(std::move(ss));
    } catch (int) {
        return EXIT_FAILURE;
    };

    return EXIT_SUCCESS;
}