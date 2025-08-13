#pragma once
#include <cassert>
#include <cstdint>
#include <iostream>
#include <optional>
#include <unordered_map>
#include <unordered_set>

#include "acclexer.hpp"
#define LEX_TYPE_TEST_FAILURE false

/**
 * Dev flags
 * -Ace-verbose-lexer
 * -Ace-verbose-ast
 *
 *
 * e.g.
 *
 *
 *
 * (ace protocol) (dev flags)    (verbose lexer) (verbose ast)  (input file)  (output type flag (binary))     (executable name)
 * acc             --set-dev    [-verbose-lexer -verbose-ast ]  f.ace         -o                              fexec
 *
 * (ace protocol)   (help)
 * acc              --help
 */
namespace acc {

class cli {
    enum class COMMANDS : std::uint8_t {
        ACE_PROTOCOL = 1 << 1,
        DEV_FLAGS = 1 << 2,
        HELP = 1 << 3,
        VERBOSE_LEXER = 1 << 4,
        VERBOSE_AST = 1 << 5
    };
    class cli_tiny_parser {
       private:
        std::vector<acc::token> tokens;
        const std::unordered_map<COMMANDS, std::string>
            m_commands{
                {COMMANDS::ACE_PROTOCOL, "acc"},
                {COMMANDS::DEV_FLAGS, "--set-dev"},
                {COMMANDS::HELP, "--help"},
                {COMMANDS::VERBOSE_LEXER, "-verbose-lexer"},
                {COMMANDS::VERBOSE_AST, "-verbose-ast"},
            };

       public:
        cli_tiny_parser() : tokens{} {};
        cli_tiny_parser(const std::vector<acc::token>& toks) : tokens{toks} {};
        cli_tiny_parser(const cli_tiny_parser& ctp) : tokens(ctp.tokens) {};
        cli_tiny_parser(cli_tiny_parser&& ctp) : tokens(std::move(ctp.tokens)) {};
        ~cli_tiny_parser() {};

        std::uint8_t get_flags() {
            std::uint8_t m_build_flags{};
            if (tokens[0].word == m_commands.at(COMMANDS::ACE_PROTOCOL)) {
                m_build_flags |= (std::uint8_t)COMMANDS::ACE_PROTOCOL;
                // in ace protocol
            }
            return m_build_flags;
        };
    };

   public:
    cli(const std::string& source) {
        auto cli_lexer = acc::lexer{std::unordered_set<acc::ACC_ALL_TOKEN_ENUM>{
            acc::ACC_ALL_TOKEN_ENUM::TK_BRACE_L,
            acc::ACC_ALL_TOKEN_ENUM::TK_BRACE_R,
            acc::ACC_ALL_TOKEN_ENUM::TK_DASH,
            acc::ACC_ALL_TOKEN_ENUM::TK_DOT,
        }};

        auto tokens = cli_lexer(std::move(source));
        auto cli_parser = cli_tiny_parser(std::move(tokens));
        auto build_flags = cli_parser.get_flags();

        if (((build_flags & (std::uint8_t)COMMANDS::ACE_PROTOCOL) == (std::uint8_t)COMMANDS::ACE_PROTOCOL)) {
            if ((build_flags & (std::uint8_t)COMMANDS::VERBOSE_LEXER) == (std::uint8_t)COMMANDS::VERBOSE_LEXER) {
                for (auto& t : tokens) {
                    t.print_token();
                }
            }
        } else {
            // error unknown protocol
        }
    };
};
}  // namespace acc
