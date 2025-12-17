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
#include "utils/lexeme_inspector.hpp"

namespace acc {
// the two basic units of a set of tokens is an identifier and a number everything else is up to the user.

class acc_lexer : protected acc::fsm_storage<std::basic_string_view<char>> {
    std::size_t m_x{0};
    std::size_t m_y{0};
    std::vector<token> m_output;
    using variant_type = std::variant<std::string, char>;
    std::unordered_map<variant_type, acc::globals::meta_info> token_map;

    std::string to_substr() const noexcept {
        return std::string(this->m_input.substr(this->m_start, this->m_end - this->m_start));
    };

    std::string to_substr(std::size_t new_begin, std::size_t new_end) const noexcept {
        return std::string(this->m_input.substr(new_begin, new_end - new_begin));
    };

    token lex_identifier() {
        while (!this->is_end() && !acc::utils::lexeme_inspector::is_delim(this->peek()) && !isspace(this->peek())) {
            this->advance();
        }

        auto id = to_substr();
        if (acc::utils::lexeme_inspector::is_reserved(id)) {
            return token{
                id,
                std::make_pair(m_x, m_y),
                acc::utils::lexeme_inspector::is_type(id) ? acc::GLOBAL_TOKENS::TK_RESERVED_TYPE : TK_RESERVED,
                id};
        }

        return token{id,
                     std::make_pair(m_x, m_y),
                     acc::GLOBAL_TOKENS::TK_IDENTIFIER,
                     id};
    };

    token lex_number() {
        while (!this->is_end() && !isalpha(this->peek()) && !acc::utils::lexeme_inspector::is_delim(this->peek())) {
            this->advance();
        }

        if (this->peek() == '.') {
            this->advance();
            while (!this->is_end() && !isalpha(this->peek()) && !acc::utils::lexeme_inspector::is_delim(this->peek())) {
                this->advance();
            }
            return token{to_substr(),
                         std::make_pair(m_x, m_y),
                         acc::GLOBAL_TOKENS::TK_LITERAL_FLOAT,
                         std::stof(to_substr())};
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
        if (acc::utils::lexeme_inspector::is_delim(this->peek())) {
            if (acc::utils::lexeme_inspector::is_pair_delim(this->peek(), this->peek_next())) {
                advance();
                advance();
                auto word = to_substr(this->m_start, this->m_end);
                return token{word,
                             std::make_pair(m_x, m_y),
                             acc::utils::lexeme_inspector::to_kind(word),
                             word};
            }

            if (this->peek() == '\"') {
                return lex_string();
            }
            if (this->peek() == '\'') {
                advance();
                auto c = advance();
                if (this->peek() != '\'') throw std::runtime_error("unrecognized character");
                advance();
                return token{to_substr(this->m_start, this->m_end + 1),
                             std::make_pair(m_x, m_y),
                             acc::GLOBAL_TOKENS::TK_LITERAL_CHAR,
                             c};
            }

            return token{to_substr(this->m_start, this->m_end + 1),
                         std::make_pair(m_x, m_y),
                         acc::utils::lexeme_inspector::to_kind(this->advance()),
                         this->peek_prev()};
        };
        if (isdigit(this->peek())) {
            return lex_number();
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
    std::vector<token> ret{};

   public:
    acc_lexer() {};
    acc_lexer(
        std::string_view sv,
        const std::unordered_map<std::variant<std::string, char>, acc::globals::meta_info>& tmap)
        : acc::fsm_storage<std::string_view>(sv),
          token_map(tmap) {};
    [[nodiscard]] const std::vector<token>& view_tokens() const noexcept {
        return ret;
    }
    std::vector<token> lex() {
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

    ~acc_lexer() {};
};
}  // namespace acc