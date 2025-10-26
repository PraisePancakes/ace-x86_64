#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "acctoken.hpp"
#include "statics/ro_tokenmap.hpp"
#include "statics/tkxmacro.hpp"
#include "storage.hpp"

namespace acc {
// the two basic units of a set of tokens is an identifier and a number everything else is up to the user.

class lexer : protected acc::fsm_storage<std::basic_string_view<char>> {
    std::size_t m_x{0};
    std::size_t m_y{0};
    std::vector<token> m_output;
    using variant_type = std::variant<std::string, char>;
    std::unordered_map<variant_type, std::uint64_t> token_map;

    [[nodiscard]] bool is_reserved(std::string keyword) const noexcept {
        if (token_map.find(keyword) == token_map.end()) return false;
        return ((token_map.find(keyword)->second & acc::globals::token_flags_::FLAG_RESERVED) == acc::globals::token_flags_::FLAG_RESERVED);
    }
    [[nodiscard]] bool is_type(std::string type) const noexcept {
        if (token_map.find(type) == token_map.end()) return false;
        return ((token_map.find(type)->second & acc::globals::token_flags_::FLAG_RESERVED_TYPE) == acc::globals::token_flags_::FLAG_RESERVED_TYPE);
    }

    [[nodiscard]] bool is_pair_delim(const char unit1, const char unit2) const noexcept {
        if (token_map.find(std::string({unit1, unit2})) == token_map.end()) return false;
        return ((token_map.find(std::string({unit1, unit2}))->second & acc::globals::token_flags_::FLAG_DELIM_PAIR) == acc::globals::token_flags_::FLAG_DELIM_PAIR);
    }

    [[nodiscard]] bool is_delim(const char unit) const noexcept {
        if (token_map.find(unit) == token_map.end()) return false;
        return (((token_map.find(unit))->second & acc::globals::token_flags_::FLAG_DELIM) == acc::globals::token_flags_::FLAG_DELIM);
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
                             acc::GLOBAL_TOKENS(token_map.find(word)->second >> acc::globals::TOKEN_TYPE_SHIFTER),
                             word};
            }
            return token{to_substr(this->m_start, this->m_end + 1),
                         std::make_pair(m_x, m_y),
                         acc::GLOBAL_TOKENS(token_map.find(this->advance())->second >> acc::globals::TOKEN_TYPE_SHIFTER),
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
    void skip_comments() {
        if (this->peek() == '/') {
            if (this->peek_next() == '/') {
                advance();
                advance();
                while (!this->is_end() && this->peek() != '\n') {
                    advance();
                }
            } else if (this->peek_next() == '*') {
                advance();
                advance();
                while (!this->is_end() && this->peek() != '*' && this->peek_next() != '/') {
                    advance();
                }
                advance();
                advance();
            }
            this->m_start = this->m_end;
        }
    }

   public:
    lexer() {};
    lexer(
        std::string_view sv,
        const std::unordered_map<std::variant<std::string, char>, std::uint64_t>& tmap)
        : acc::fsm_storage<std::string_view>(sv),
          token_map(tmap) {
          };

    std::vector<token> lex() {
        std::cout << "IN LEX\n";
        std::vector<token> ret{};
        while (!this->is_end()) {
            skip_comments();
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