#pragma once
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "acctoken.hpp"
#include "statics/tkxmacro.hpp"
#include "storage.hpp"

namespace acc {
// the two basic units of a set of tokens is an identifier and a number everything else is up to the user.

template <typename T = char>
class lexer : protected acc::fsm_storage<std::basic_string_view<char>> {
    std::size_t m_x{0};
    std::size_t m_y{0};
    std::vector<token> m_output;
    std::unordered_set<GLOBAL_TOKENS> m_delims;
    std::unordered_map<std::string, acc::GLOBAL_TOKENS> m_pair_delims;
    std::unordered_set<std::string> m_reserved;

    [[nodiscard]] bool is_reserved(std::string keyword) const noexcept {
        return m_reserved.find(keyword) != m_reserved.end();
    }
    [[nodiscard]] bool is_type(std::string type) const noexcept {
        return m_types.find(type) != m_types.end();
    }

    [[nodiscard]] bool is_pair_delim(const char unit1, const char unit2) const noexcept {
        return m_pair_delims.find(std::string({unit1, unit2})) != m_pair_delims.end();
    }

    [[nodiscard]] bool is_delim(const unsigned char unit) const noexcept {
        return m_delims.find((GLOBAL_TOKENS)unit) != m_delims.end();
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

        auto id = to_substr();
        if (is_reserved(id)) {
            return token{
                id,
                std::make_pair(m_x, m_y),
                is_type(id) ? acc::GLOBAL_TOKENS::TK_RESERVED_TYPE : TK_RESERVED,
                id};
        }

        return token{id,
                     std::make_pair(m_x, m_y),
                     acc::GLOBAL_TOKENS::TK_IDENTIFIER,
                     id};
    };

    token lex_number() {
        while (!this->is_end() && !isalpha(this->peek()) && !is_delim(this->peek())) {
            this->advance();
        }
        return token{to_substr(),
                     std::make_pair(m_x, m_y),
                     acc::GLOBAL_TOKENS::TK_LITERAL_INT,
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
                     acc::GLOBAL_TOKENS::TK_LITERAL_STRING,
                     to_substr()};
    };

    token lex_it() {
        if (is_delim(this->peek())) {
            // NOTE:  potential pitfall if pair delimeter cannot be separated into two single delims.
            if (is_pair_delim(this->peek(), this->peek_next())) {
                advance();
                advance();
                auto word = to_substr(this->m_start, this->m_end);
                return token{word,
                             std::make_pair(m_x, m_y),
                             m_pair_delims[word],
                             word};
            }
            return token{to_substr(this->m_start, this->m_end + 1),
                         std::make_pair(m_x, m_y),
                         *(m_delims.find((acc::GLOBAL_TOKENS)this->advance())),
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
    std::unordered_set<std::string> m_types;

   public:
    lexer() {};
    lexer(
        std::string_view sv,
        const std::unordered_set<acc::GLOBAL_TOKENS>& delims,
        const std::unordered_map<std::string, acc::GLOBAL_TOKENS>& pair_delims,
        const std::unordered_set<std::string>& reserves, const std::unordered_set<std::string>& types)
        : acc::fsm_storage<std::string_view>(sv),
          m_delims(delims),
          m_pair_delims(pair_delims),
          m_reserved(reserves),
          m_types(types) {
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