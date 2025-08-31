#pragma once
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "acctoken.hpp"
#include "tkxmacro.hpp"
namespace acc {

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

    token lex_string() {
        advance();
        while (!is_end() && peek() != '\"') {
            advance();
        }
        if (is_end()) {
            throw std::runtime_error("open string");
        }
        advance();
        return token{to_substr(),
                     std::make_pair(m_x, m_y),
                     token_type_t::TK_LITERAL_STRING,
                     to_substr()};
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
        if (peek() == '\"') {
            return lex_string();
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