#pragma once
#include <cstdint>
#include <iostream>
#include <unordered_map>
/**
 * Dev flags
 * -Ace-verbose-lexer
 * -Ace-verbose-ast
 *
 *
 * e.g.
 * acc --set-dev [-verbose-lexer -verbose-ast ] f.ace
 * acc --help
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

    std::uint8_t m_build_flags{};

   public:
    cli(const std::string& source) {
        std::cout << source;
    };
};
}  // namespace acc
