#pragma once
#include <cstdint>
#include <iostream>
#include <unordered_map>

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

    enum class CLI_TOKENS {
        LBRACE,
        RBRACE,
        DASH,
        IDENTIFIER,
        NUMBER,
        DOT
    };

#if LEX_TYPE_TEST_FAILURE
    enum class ASSERTED_LEX_TYPE_FAILURE {};
#endif
    const std::unordered_map<COMMANDS, std::string>
        m_commands{
            {COMMANDS::ACE_PROTOCOL, "acc"},
            {COMMANDS::DEV_FLAGS, "--set-dev"},
            {COMMANDS::HELP, "--help"},
            {COMMANDS::VERBOSE_LEXER, "-verbose-lexer"},
            {COMMANDS::VERBOSE_AST, "-verbose-ast"},
        };

    std::uint8_t m_build_flags{};

   public:
    cli(const std::string& source) {
        auto cli_lexer = acc::lexer{std::unordered_map<unsigned char, CLI_TOKENS>{
            {'[', CLI_TOKENS::LBRACE},
            {']', CLI_TOKENS::RBRACE},
            {'-', CLI_TOKENS::DASH},
            {'.', CLI_TOKENS::DOT}}};
        static_assert(std::is_same_v<decltype(cli_lexer)::token_type, CLI_TOKENS>);
#if LEX_TYPE_TEST_FAILURE
        static_assert(!std::is_same_v<decltype(cli_lexer)::token_type, ASSERTED_LEX_TYPE_FAILURE>);
#endif
        auto tokens = cli_lexer(std::move(source));
        for (auto& t : tokens) {
            std::cout << "[TOKEN] : " << t.word << " [TYPE]     " << (int)t.type << std::endl;
        }
    };
};
}  // namespace acc
