#pragma once
#include <iostream>
#include <string>
#include <unordered_set>
#include <variant>
#include <vector>

#include "tkxmacro.hpp"
#include "visitor.hpp"
namespace acc {

struct [[nodiscard]] token {
    using value_type = std::variant<char, int, float, std::string, double, bool>;

    std::string word;
    std::pair<int, int> location;
    using token_type_t = acc::ACC_ALL_TOKEN_ENUM;
    token_type_t type;
    value_type value;

    constexpr static inline const char* to_string(token_type_t type) noexcept {
        switch (type) {
#define TOKEN_DEF(NAME, VALUE) \
    case NAME:                 \
        return #NAME;
            TOKEN_DEFs
#undef TOKEN_DEF
                default : return "UNKNOWN_TYPE";
        }
    }

    constexpr static inline const char* to_stringized(token_type_t type) noexcept {
        switch (type) {
#define TOKEN_DEF(NAME, VALUE) \
    case NAME:                 \
        return #VALUE;
            TOKEN_DEFs
#undef TOKEN_DEF
                default : return "UNKNOWN_LITERAL";
        }
    }

    constexpr static inline std::string to_literal(token_type_t type) noexcept {
        const char* lit = to_stringized(type);
        std::string ret{lit};
        return ret.substr(1, ret.size() - 2);
    }

    void print_token() const noexcept {
        std::cout << "TOKEN TYPE ID (" << to_literal(type) << ")"
                  << " [" << to_string(type) << "]\n";
        std::cout << "location (row, col) < " << location.first << " , " << location.second << " > ";
        std::visit(internal::visitor{
                       [](char c) { std::cout << "[CHAR] " << c << std::endl; },
                       [](std::string s) { std::cout << "[STRING] " << s << std::endl; },
                       [](int i) { std::cout << "[INT] " << i << std::endl; },
                       [](float f) { std::cout << "[FLOAT] " << f << std::endl; },
                       [](double d) { std::cout << "[DOUBLE] " << d << std::endl; },
                       [](bool b) { std::cout << "[BOOL] " << std::boolalpha << b << std::endl; }},
                   value);
    };

    token() = default;
    token(const std::string& word, token_type_t type, value_type val) : word(word), location(0, 0), type(type), value(val) {};
    token(const std::string& word, std::pair<int, int> l, token_type_t type, value_type val) : word(word), location(l), type(type), value(val) {};
    token(const std::string& word, int x, int y, token_type_t type, value_type val) : word(word), location(std::make_pair(x, y)), type(type), value(val) {};
    token(const token& o) : word(o.word), location(o.location), type(o.type), value(o.value) {};
    token(token&& o) : word(std::move(o.word)), location(o.location), type(o.type), value(std::move(o.value)) {};
    token& operator=(const token& o) {
        this->word = o.word;
        this->location = o.location;
        this->type = o.type;
        return *this;
    };
    ~token() = default;
};

// the two basic units of a set of tokens is an identifier and a number everything else is up to the user.

class lexer {
    std::unordered_set<ACC_ALL_TOKEN_ENUM> m_delims;
    std::size_t m_x{0};
    std::size_t m_y{0};
    std::size_t m_start{0};
    std::size_t m_end{0};
    std::vector<token> m_output;
    std::string_view m_input;

    [[nodiscard]] bool is_end() const noexcept {
        return m_end >= m_input.size();
    };

    [[nodiscard]] char peek() const noexcept {
        return m_input[m_end];
    };

    [[nodiscard]] char peek_prev() const noexcept {
        return m_input[m_end - 1];
    }

    [[nodiscard]] char peek_next() const noexcept {
        return m_input[m_end + 1];
    }

    char advance() noexcept {
        if (is_end()) {
            return peek();
        }
        return m_input[m_end++];
    };

    [[nodiscard]] bool is_delim(const unsigned char unit) const noexcept {
        return m_delims.find((ACC_ALL_TOKEN_ENUM)unit) != m_delims.end();
    }

    std::string to_substr() const noexcept {
        return std::string(m_input.substr(m_start, m_end - m_start));
    };

    std::string to_substr(std::size_t new_begin, std::size_t new_end) const noexcept {
        return std::string(m_input.substr(new_begin, new_end - new_begin));
    };

    token lex_identifier() {
        while (!is_end() && !is_delim(peek()) && !isspace(peek())) {
            advance();
        }
        return token{to_substr(),
                     std::make_pair(m_x, m_y),
                     token_type_t::TK_IDENTIFIER,
                     to_substr()};
    };

    token lex_number() {
        while (!is_end() && !isalpha(peek()) && !is_delim(peek())) {
            advance();
        }
        return token{to_substr(),
                     std::make_pair(m_x, m_y),
                     token_type_t::TK_LITERAL_INT,
                     std::stoi(to_substr())};
    };

    token lex_it() {
        if (is_delim(peek())) {
            return token{to_substr(m_start, m_end + 1),
                         std::make_pair(m_x, m_y),
                         *(m_delims.find((acc::ACC_ALL_TOKEN_ENUM)advance())),
                         peek_prev()};
        };
        if (isdigit(peek())) {
            return lex_number();
        }
        return lex_identifier();
    };

   public:
    using token_type_t = acc::ACC_ALL_TOKEN_ENUM;
    lexer() {};
    lexer(const std::unordered_set<acc::ACC_ALL_TOKEN_ENUM>& delims) : m_delims(delims) {

                                                                       };

    std::vector<token> operator()(std::string_view sv) noexcept {
        m_input = sv;
        std::vector<token> ret{};

        while (!is_end()) {
            m_x++;
            if (peek() == '\n') {
                m_x = 0;
                m_y++;
            }

            if (std::isspace(peek())) {
                advance();
                m_start = m_end;
                continue;
            }
            ret.push_back(lex_it());
            m_start = m_end;
        }
        ret.push_back(token{"", acc::ACC_ALL_TOKEN_ENUM::TK_EOF, '\0'});
        return ret;
    };
    ~lexer() {};
};
}  // namespace acc