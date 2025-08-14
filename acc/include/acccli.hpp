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
            return std::unexpected("parser error");
        };
    };
};

parser<int> digit_(char t) {
    return [=](std::istream& s) -> result<int> {
        char v = s.get();
        if (std::isdigit((unsigned char)v) && v == t) {
            int x = v - '0';
            return x;
        } else {
            s.unget();
            return std::unexpected("parser error");
        }
    };
};

parser<int> number_() {
    return [=](std::istream& ss) -> result<int> {
        std::string v = "";
        while (true) {
            auto val = ss.get();
            if (digit_(val) && !ss.eof()) {
                v += val;
            } else {
                break;
            }
        }
        return std::stoi(v);
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
