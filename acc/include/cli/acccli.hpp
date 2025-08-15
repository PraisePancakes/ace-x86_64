#pragma once
#include <cassert>
#include <cstdint>
#include <iostream>

#include "../acclog.hpp"
#include "prs_combinator.hpp"

/**
 *  BNF
 * ______
 *
 * <major> ::= <dash><dash><ident><minor>
 * <minor> ::= <dash><ident>
 * <ident> ::= <string>
 *
 */
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

class cli {
    enum class COMMANDS : std::uint8_t {
        ACE_PROTOCOL = 1 << 0,
        DEV_FLAGS = 1 << 1,
        HELP_DEV = 1 << 2,
        HELP_ALL = 1 << 3,
        VERBOSE_LEXER = 1 << 4,
        VERBOSE_AST = 1 << 5
    };

    std::uint8_t m_build_flags{0};

    void parse_help_minor(std::stringstream& ss) {
        if (acc::match_("-")(ss)) {
            if (acc::match_("all")(ss)) {
                m_build_flags |= (std::uint8_t)COMMANDS::HELP_ALL;
            }
        }
    }

    void parse_set_minor(std::stringstream& ss) {
        acc::many_(acc::ignore_(acc::match_(' ')))(ss);
        if (acc::match_("-")(ss).has_value()) {
            if (acc::match_("dev")(ss)) {
            };
        }
    };

    void parse_major(std::stringstream& ss) {
        acc::many_(acc::ignore_(acc::match_(' ')))(ss);
        if (acc::match_("--")(ss).has_value()) {
            if (acc::match_("help")(ss).has_value()) {
                parse_help_minor(ss);
                return;
            }

            if (acc::match_("set")(ss).has_value()) {
                parse_set_minor(ss);
                return;
            }
        }
    };

    void parse(std::stringstream& ss) {
        if (acc::match_("acc")(ss).has_value()) {
            m_build_flags |= (std::uint8_t)COMMANDS::ACE_PROTOCOL;
            parse_major(ss);
        }
    };

    void print_usage_all() {
        acc::logger::instance().send(logger::LEVEL::INFO, "(ace protocol) (dev flags)     (verbose lexer) (verbose ast)  (input file)  (output type flag (binary))     (executable name)");
        acc::logger::instance().send(logger::LEVEL::INFO, " acc              --set-dev    [-verbose-lexer -verbose-ast ]  f.ace         -o                              fexec");
        acc::logger::instance().send(logger::LEVEL::INFO, " acc              --help-all");
        acc::logger::instance().send(logger::LEVEL::INFO, " acc              --help-dev");
    };

    [[nodiscard]] bool is_set(COMMANDS flag) {
        return ((m_build_flags & std::underlying_type_t<COMMANDS>(flag)) == std::underlying_type_t<COMMANDS>(flag));
    };

   public:
    cli(std::stringstream&& ss) {
        parse(ss);

        if (is_set(COMMANDS::ACE_PROTOCOL)) {
            if (is_set(COMMANDS::HELP_ALL)) {
                print_usage_all();
            }
        }
    };
};
}  // namespace acc
