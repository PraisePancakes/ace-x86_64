#pragma once
#include <array>
#include <iostream>
#include <vector>

#include "../traits/token_trait.hpp"
#include "../utils/visitor.hpp"
#include "accast.hpp"
#include "accprinter.hpp"
#include "acctoken.hpp"
#include "storage.hpp"

#define DISCARD(f) (void)f
namespace acc {

class [[nodiscard]] acc_parser
    : public acc::fsm_storage<std::vector<acc::token>> {
    std::vector<acc::ExprVariant> exprs;  // block will have this

    bool check_it(acc::ACC_ALL_TOKEN_ENUM type) noexcept {
        if (this->is_end()) {
            return false;
        }
        return this->peek().type == type;
    }
    bool match_it(acc::ACC_ALL_TOKEN_ENUM type) noexcept {
        if (check_it(type)) {
            DISCARD(this->advance());
            return true;
        }

        return false;
    }

    bool match_any(traits::acc_token_t auto&&... types) {
        return (match_it(types) || ...);
    }

    bool match_seq(traits::acc_token_t auto&&... types) {
        auto old = this->m_end;
        if ((!match_it(types) && ...)) {
            this->m_end = old;
            return false;
        };
        return true;
    }

    acc::ExprVariant parse_primary() {
        if (match_any(TK_LITERAL_INT,
                      TK_LITERAL_STRING,
                      TK_LITERAL_DOUBLE,
                      TK_LITERAL_FLOAT)) {
            return new acc::node::LiteralExpr{.value = this->peek_prev().value,
                                              .embedded = this->peek_prev()};
        };

        if (match_it(TK_PAREN_L)) {
            auto expr = parse_expr();
            if (!match_it(TK_PAREN_R)) throw std::runtime_error("MISSING PAREN");
            return new acc::node::GroupingExpr{.expr = expr};
        }

        throw std::runtime_error("PARSERROR");
    }

    acc::ExprVariant parse_unary() {
        if (match_any(TK_BANG, TK_STAR)) {
            auto op = this->peek_prev();
            auto expr = parse_expr();
            return new acc::node::UnaryExpr{.op = op, .expr = expr};
        }
        return parse_primary();
    };

    acc::ExprVariant parse_term() {
        auto lhs = parse_unary();
        while (match_any(acc::ACC_ALL_TOKEN_ENUM::TK_PLUS,
                         acc::ACC_ALL_TOKEN_ENUM::TK_DASH)) {
            auto op = this->peek_prev();
            auto rhs = parse_expr();
            return new acc::node::BinaryExpr{.lhs = lhs,
                                             .rhs = rhs,
                                             .op = op};
        };
        return lhs;
    };
    acc::ExprVariant parse_factor() {
        auto lhs = parse_term();
        while (match_any(acc::ACC_ALL_TOKEN_ENUM::TK_STAR,
                         acc::ACC_ALL_TOKEN_ENUM::TK_SLASH)) {
            auto op = this->peek_prev();
            auto rhs = parse_expr();
            return new acc::node::BinaryExpr{.lhs = lhs,
                                             .rhs = rhs,
                                             .op = op};
        };
        return lhs;
    };

    acc::ExprVariant parse_comparison() {
        auto lhs = parse_factor();
        while (match_any(acc::ACC_ALL_TOKEN_ENUM::TK_BANG_EQ,
                         acc::ACC_ALL_TOKEN_ENUM::TK_LT,
                         acc::ACC_ALL_TOKEN_ENUM::TK_GT,
                         acc::ACC_ALL_TOKEN_ENUM::TK_LT_EQ,
                         acc::ACC_ALL_TOKEN_ENUM::TK_GT_EQ,
                         acc::ACC_ALL_TOKEN_ENUM::TK_STRICT_EQ)) {
            auto op = this->peek_prev();
            auto rhs = parse_expr();
            return new acc::node::ComparisonExpr{.lhs = lhs,
                                                 .rhs = rhs,
                                                 .op = op};
        }
        return lhs;
    };

    acc::ExprVariant parse_expr() {
        return parse_comparison();
    };

   public:
    acc_parser(const std::vector<acc::token>& toks)
        : acc::fsm_storage<std::vector<acc::token>>(toks) {

          };
    void print_ast() {
        acc::printer printer(exprs);
        printer.print();
    };

    std::vector<acc::ExprVariant> parse() {
        do {
            exprs.push_back(parse_expr());
        } while (!this->is_end());

        return exprs;
    };
    acc_parser(const acc_parser&) = delete;
    acc_parser& operator=(const acc_parser&) = delete;
    acc_parser(acc_parser&&) = default;
    acc_parser& operator=(acc_parser&&) = default;
    ~acc_parser() = default;
};
}  // namespace acc