#pragma once
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "acctoken.hpp"
#include "storage.hpp"
#include "tkxmacro.hpp"
namespace acc {

// the two basic units of a set of tokens is an identifier and a number everything else is up to the user.

template <typename T = char>
class lexer : protected acc::fsm_storage<std::basic_string_view<char>> {
    std::size_t m_x{0};
    std::size_t m_y{0};
    std::vector<token> m_output;
    std::unordered_set<ACC_ALL_TOKEN_ENUM> m_delims;
    [[nodiscard]] bool is_delim(const unsigned char unit) const noexcept {
        return m_delims.find((ACC_ALL_TOKEN_ENUM)unit) != m_delims.end();
    }

    std::string to_substr() const noexcept {
        return std::string(this->m_input.substr(this->m_start, this->m_end - this->m_start));
    };

    std::string to_substr(std::size_t new_begin, std::size_t new_end) const noexcept {
        return std::string(this->m_input.substr(new_begin, new_end - new_begin));
    };

    token lex_identifier() {
        while (!this->is_end() && !is_delim(this->peek()) && !isspace(this->peek())) {
            this->advance();
        }
        return token{to_substr(),
                     std::make_pair(m_x, m_y),
                     token_type_t::TK_IDENTIFIER,
                     to_substr()};
    };

    token lex_number() {
        while (!this->is_end() && !isalpha(this->peek()) && !is_delim(this->peek())) {
            this->advance();
        }
        return token{to_substr(),
                     std::make_pair(m_x, m_y),
                     token_type_t::TK_LITERAL_INT,
                     std::stoi(to_substr())};
    };

    token lex_string() {
        this->advance();
        while (this->peek() != '\"' && !this->is_end()) {
            this->advance();
        }
        if (this->is_end()) {
            throw std::runtime_error("LEXERROR : unbounded string");
        }
        this->advance();

        return token{to_substr(),
                     std::make_pair(m_x, m_y),
                     token_type_t::TK_LITERAL_STRING,
                     to_substr()};
    };

    token lex_it() {
        if (is_delim(this->peek())) {
            return token{to_substr(this->m_start, this->m_end + 1),
                         std::make_pair(m_x, m_y),
                         *(m_delims.find((acc::ACC_ALL_TOKEN_ENUM)this->advance())),
                         this->peek_prev()};
        };
        if (isdigit(this->peek())) {
            return lex_number();
        }
        if (this->peek() == '\"') {
            return lex_string();
        }
        return lex_identifier();
    };

   public:
    using token_type_t = acc::ACC_ALL_TOKEN_ENUM;
    lexer() {};
    lexer(const std::unordered_set<acc::ACC_ALL_TOKEN_ENUM>& delims, std::string_view sv) : acc::fsm_storage<std::string_view>(sv), m_delims(delims) {

                                                                                            };
    std::vector<token> lex() {
        std::vector<token> ret{};
        while (!this->is_end()) {
            m_x++;
            if (this->peek() == '\n') {
                m_x = 0;
                m_y++;
            }

            if (std::isspace(this->peek())) {
                this->advance();
                this->m_start = this->m_end;
                continue;
            }
            ret.push_back(lex_it());
            this->m_start = this->m_end;
        }
        return ret;
    }

    ~lexer() {};
};
}  // namespace acc