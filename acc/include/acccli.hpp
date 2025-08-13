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
        ACE_PROTOCOL,
        DEV_FLAGS,
        HELP,
        VERBOSE_LEXER,
        VERBOSE_AST
    };

    const std::unordered_map<COMMANDS, std::string>
        m_commands{
            {COMMANDS::ACE_PROTOCOL, "acc"},
            {COMMANDS::DEV_FLAGS, "--set-dev"},
            {COMMANDS::HELP, "--help"},
            {COMMANDS::VERBOSE_LEXER, "-verbose-lexer"},
            {COMMANDS::VERBOSE_AST, "-verbose-ast"},
        };

    std::optional<std::uint8_t> cli_tiny_parser(const std::vector<acc::token>& toks) {
        if (toks[0].word == m_commands.at(COMMANDS::ACE_PROTOCOL)) {
            std::uint8_t m_build_flags{};
            // in ace protocol
            
            return m_build_flags;
        }
        return std::nullopt;
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

        // eventually determined by build flags
        for (auto& t : tokens) {
            t.print_token();
        }
    };
};
}  // namespace acc
