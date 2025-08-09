#include <cstdint>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>
/**
 * Dev flags
 * -Ace-verbose-lexer
 * -Ace-verbose-ast
 *
 *
 * e.g.
 * ace -Ace-dev-flags [--verbose-lexer --verbose-ast --verbose-cli ] f.ace
 */

struct [[nodiscard]] ace_lexeme {
    std::string word;
    std::pair<int, int> location;
    ace_lexeme() = default;
    ace_lexeme(const std::string& word) : word(word), location(0, 0) {};
    ace_lexeme(const std::string& word, std::pair<int, int> l) : word(word), location(l) {};
    ace_lexeme(const ace_lexeme& o) : word(o.word), location(o.location) {};
    ace_lexeme(ace_lexeme&& o) : word(std::move(o.word)), location(o.location) {};
    ~ace_lexeme() = default;
};

class ace_lexer {
    std::string output;

   public:
    ace_lexer() {};
    ace_lexer(std::string_view sv) {};
    ~ace_lexer() {};
};

class ace_cli {
    enum class COMMANDS : std::uint8_t {
        ACE_PROTOCOL,
        DEV_FLAGS,
        VERBOSE_LEXER,
        VERBOSE_AST
    };

    const std::unordered_map<COMMANDS, std::string>
        commands{
            {COMMANDS::ACE_PROTOCOL, "ace"},
            {COMMANDS::DEV_FLAGS, "-Ace-dev-flags"},
            {COMMANDS::VERBOSE_LEXER, "--verbose-lexer"},
            {COMMANDS::VERBOSE_AST, "--verbose-ast"},
        };

    std::uint8_t build_flags{};

   public:
    ace_cli(const std::string& source) {
        std::cout << source;
    };
};

auto main(int argc, char** argv) -> int {
    if (argc <= 1) {
        std::cout << argc << std::endl;
        exit(EXIT_FAILURE);
    };
    // skip compiler exe, look for ace protocol commands
    std::stringstream ss;
    for (int i = 1; i < argc; i++) {
        ss << std::string(argv[i]);
    }
    ace_cli acli(std::move(ss.str()));

    return 0;
}