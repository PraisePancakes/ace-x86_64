#pragma once
#include <iostream>
#include <vector>

#include "../traits/token_trait.hpp"
#include "../utils/visitor.hpp"
#include "accast.hpp"
#include "acctoken.hpp"
#include "storage.hpp"
#define DISCARD(f) (void)f
namespace acc {

template <typename T = acc::token>
class [[nodiscard]] acc_parser
    : public acc::storage<std::vector<T>> {
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

    auto match_any(traits::acc_token_t auto&&... types) {
        return (match_it(types) || ...);
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

    acc::ExprVariant parse_expr() {
        return parse_factor();
    };

   public:
    acc_parser(const std::vector<T>& toks)
        : acc::storage<std::vector<T>>(toks) {

          };

    void print_node(acc::ExprVariant expr) {
        std::visit(internal::visitor{[this](acc::node::BinaryExpr* bxpr) {
                                         std::cout << " [ BINARY EXPR ] " << std::endl;
                                         print_node(bxpr->lhs);
                                         bxpr->op.print_token();
                                         print_node(bxpr->rhs);
                                     },
                                     [](acc::node::LiteralExpr* lxpr) {
                                         std::cout << " [ LITERAL EXPR ] " << std::endl;
                                         lxpr->embedded.print_token();
                                     },
                                     [this](acc::node::UnaryExpr* uexpr) {
                                         std::cout << " [ UNARY EXPR ] " << std::endl;
                                         uexpr->op.print_token();
                                         print_node(uexpr->expr);
                                     },
                                     [this](acc::node::GroupingExpr* gexpr) {
                                         std::cout << " [ GROUP EXPR ] " << std::endl;
                                         print_node(gexpr->expr);
                                     }},
                   expr);
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