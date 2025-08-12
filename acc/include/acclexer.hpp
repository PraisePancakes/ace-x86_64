#pragma once
#include <iostream>
#include <vector>

#include "tkxmacro.hpp"
namespace acc {

template <typename TokenType>
    requires(std::is_enum_v<TokenType>)
struct [[nodiscard]] token {
    using value_type = std::variant<int, float, std::string, double, bool>;

    std::string word;
    std::pair<int, int> location;
    TokenType type;
    value_type value;

    constexpr static inline const char* to_string(TokenType type) noexcept {
        switch (type) {
#define TOKEN_DEF(NAME, VALUE) \
    case NAME:                 \
        return #NAME;
            TOKEN_DEFs
#undef TOKEN_DEF
                default : return "UNKNOWN_TYPE";
        }
    }
    token() = default;
    token(const std::string& word, TokenType type) : word(word), location(0, 0), type(type) {};
    token(const std::string& word, std::pair<int, int> l, TokenType type) : word(word), location(l), type(type) {};
    token(const std::string& word, int x, int y, TokenType type) : word(word), location(std::make_pair(x, y)), type(type) {};
    token(const token& o) : word(o.word), location(o.location), type(o.type) {};
    token(token&& o) : word(std::move(o.word)), location(o.location), type(o.type) {};
    token& operator=(const token& o) {
        this->word = o.word;
        this->location = o.location;
        this->type = o.type;
        return *this;
    };
    ~token() = default;
};

// the two basic units of a set of tokens is an identifier and a number everything else is up to the user.
template <typename TokenType>
    requires(std::is_enum_v<TokenType> && requires { TokenType::NUMBER; } && requires { TokenType::IDENTIFIER; })
class lexer {
    std::unordered_map<unsigned char, TokenType> m_delims;
    std::size_t m_x{0};
    std::size_t m_y{0};
    std::size_t m_start{0};
    std::size_t m_end{0};
    std::vector<token<TokenType>> m_output;
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
        return m_delims.find(unit) != m_delims.end();
    }

    std::string to_substr() const noexcept {
        return std::string(m_input.substr(m_start, m_end - m_start));
    };

    std::string to_substr(std::size_t new_begin, std::size_t new_end) const noexcept {
        return std::string(m_input.substr(new_begin, new_end - new_begin));
    };

    token<TokenType> lex_identifier() {
        while (!is_end() && !is_delim(peek()) && !isspace(peek())) {
            advance();
        }
        return token<TokenType>{to_substr(), std::make_pair(m_x, m_y), TokenType::IDENTIFIER};
    };

    token<TokenType> lex_number() {
        while (!is_end() && !isalpha(peek()) && !is_delim(peek())) {
            advance();
        }
        return token<TokenType>{to_substr(), std::make_pair(m_x, m_y), TokenType::NUMBER};
    };

    token<TokenType> lex_it() {
        if (is_delim(peek())) {
            return token<TokenType>{to_substr(m_start, m_end + 1), std::make_pair(m_x, m_y), m_delims[advance()]};
        };
        if (isdigit(peek())) {
            return lex_number();
        }
        return lex_identifier();
    };

   public:
    using token_type = TokenType;
    lexer() {};
    lexer(std::unordered_map<unsigned char, TokenType> delims) : m_delims(delims) {

                                                                 };

    std::vector<token<TokenType>> operator()(std::string_view sv) noexcept {
        m_input = sv;
        std::vector<token<TokenType>> ret{};

        while (!is_end()) {
            if (std::isspace(peek())) {
                advance();
                m_start = m_end;
                continue;
            }
            ret.push_back(lex_it());
            m_start = m_end;
        }
        return ret;
    };
    ~lexer() {};
};
}  // namespace acc