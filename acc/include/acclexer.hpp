#pragma once
#include <iostream>
#include <vector>

namespace acc {

template <typename TokenType>
    requires(std::is_enum_v<TokenType>)
struct [[nodiscard]] token {
    using group_type = TokenType;
    std::string word;
    std::pair<int, int> location;
    TokenType type;

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
    std::unordered_map<std::string, TokenType> m_token_map;
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

    [[nodiscard]] char advance() noexcept {
        if (is_end()) {
            return peek();
        }
        return m_input[m_end++];
    };

    [[nodiscard]] bool is_rsvp(std::string unit) const noexcept {
        return m_token_map.find(unit) != m_token_map.end();
    }

    [[nodiscard]] bool is_rsvp(const char unit) const noexcept {
        std::string u = "";
        u += unit;
        return m_token_map.find(u) != m_token_map.end();
    }

    token<TokenType> lex_identifier() {
        std::string unit = "";
        while (!is_end() && !is_rsvp(peek()) && !isspace(peek())) {
            unit += advance();
        }

        return token<TokenType>{unit, std::make_pair(m_x, m_y), TokenType::IDENTIFIER};
    };

    token<TokenType> lex_number() {
        std::string unit = "";
        while (!is_end() && !isalpha(peek()) && !is_rsvp(peek())) {
            unit += advance();
        }
        return token<TokenType>{unit, std::make_pair(m_x, m_y), TokenType::NUMBER};
    };

    token<TokenType> lex_it() {
        if (is_rsvp(peek())) {
            std::string unit = "";
            unit += advance();
            return token<TokenType>{unit, std::make_pair(m_x, m_y), m_token_map[unit]};
        };
        if (isdigit(peek())) {
            return lex_number();
        }
        return lex_identifier();
    };

   public:
    using token_type = TokenType;
    lexer() {};
    lexer(std::unordered_map<std::string, TokenType> map) : m_token_map(map) {

                                                            };

    std::vector<token<TokenType>> operator()(std::string_view sv) noexcept {
        m_input = sv;
        std::vector<token<TokenType>> ret{};

        while (!is_end()) {
            if (std::isspace(peek())) {
                (void)advance();
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