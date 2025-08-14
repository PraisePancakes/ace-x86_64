#pragma once
#include <cassert>
#include <cstdint>
#include <expected>
#include <functional>
#include <iostream>
#include <list>

#include "acclog.hpp"

/**
 * acc cli usage : [protocol]  --[major command]-[minor command]  [-[list of minor command flags], -[...]] [input file].[ace] -[output type]  [executable name]
 *
 * e.g.
 *
 *
 * two dash equals commands      one dash equals options                       output option : binary
 * (ace protocol) (dev flags)    (verbose lexer) (verbose ast)  (input file)  (output type flag (binary))     (executable name)
 * acc             --set-dev    [-verbose-lexer -verbose-ast ]  f.ace         -o                              fexec
 *
 * (ace protocol)   (help)
 * acc              --help-all
 * acc              --help-dev
 */
namespace acc {

template <typename T>
using result = std::expected<T, std::string>;

template <typename T>
struct parser : std::function<result<T>(std::istream&)> {
    using std::function<result<T>(std::istream&)>::function;
    using value_type = T;
    constexpr parser(parser<T> (*ptr)()) : parser(ptr()) {}
};

parser<char> char_(char t) {
    return [=](std::istream& s) -> result<char> {
        char v = s.get();
        if (v == t) {
            return v;
        } else {
            s.unget();
            return std::unexpected("char_ error");
        };
    };
};

parser<int> digit_() {
    return [](std::istream& s) -> result<int> {
        char v = s.get();
        if (std::isdigit((unsigned char)v)) {
            int x = v - '0';
            return x;
        } else {
            s.unget();
            return std::unexpected("digit_ error");
        }
    };
};

parser<int> int_() {
    return [](std::istream& ss) -> result<int> {
        auto digit_parser = digit_();
        std::string ret = "";

        while (true) {
            auto v = digit_parser(ss);
            if (v.has_value() && !ss.eof()) {
                ret += ('0' + v.value());
            } else {
                break;
            }
        };

        if (ret.empty()) return std::unexpected("number_ error");
        return std::stoi(ret);
    };
};

class cli {
    enum class COMMANDS : std::uint8_t {
        ACE_PROTOCOL = 1 << 1,
        DEV_FLAGS = 1 << 2,
        HELP = 1 << 3,
        VERBOSE_LEXER = 1 << 4,
        VERBOSE_AST = 1 << 5
    };

   public:
    cli(std::stringstream& ss) {

    };
};
}  // namespace acc
