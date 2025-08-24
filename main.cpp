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
    ss << "hello";

    auto v = acc::map(acc::match_("hello"), [](const std::string& st) { int s = 0;
                                                                        for(auto e : st) {
                                                                            std::cout << (unsigned char)e << " : " << (int)e << std::endl;
                                                                            s += e;
                                                                        }
                                                                        return s; })(ss);

    std::cout << v.value_or(3);

    return EXIT_SUCCESS;
}