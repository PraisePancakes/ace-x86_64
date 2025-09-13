#pragma once
#include <array>
#include <iostream>
#include <utility>
#include <vector>

#include "../traits/token_trait.hpp"
#include "../utils/visitor.hpp"
#include "accast.hpp"
#include "accprinter.hpp"
#include "acctoken.hpp"
#include "statics/ro_acctypes.hpp"
#include "storage.hpp"

#define DISCARD(f) (void)f
namespace acc {

class [[nodiscard]] acc_parser
    : public acc::fsm_storage<std::vector<acc::token>> {
    std::vector<acc::StmtVariant> stmts;
    bool check_it(acc::GLOBAL_TOKENS type) noexcept {
        if (this->is_end()) {
            return false;
        }
        return this->peek().type == type;
    }
    bool match_it(acc::GLOBAL_TOKENS type) noexcept {
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
        while (match_any(acc::GLOBAL_TOKENS::TK_PLUS,
                         acc::GLOBAL_TOKENS::TK_DASH)) {
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
        while (match_any(acc::GLOBAL_TOKENS::TK_STAR,
                         acc::GLOBAL_TOKENS::TK_SLASH)) {
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
        while (match_any(acc::GLOBAL_TOKENS::TK_BANG_EQ,
                         acc::GLOBAL_TOKENS::TK_LT,
                         acc::GLOBAL_TOKENS::TK_GT,
                         acc::GLOBAL_TOKENS::TK_LT_EQ,
                         acc::GLOBAL_TOKENS::TK_GT_EQ,
                         acc::GLOBAL_TOKENS::TK_STRICT_EQ)) {
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

    acc::StmtVariant parse_expression_statement() {
        return new acc::node::ExpressionStmt{.expr = parse_expr()};
    }

    acc::StmtVariant parse_declaration() {
        if (match_it(acc::GLOBAL_TOKENS::TK_RESERVED)) {
            if (acc::globals::ACC_TYPE_SET.contains(this->peek_prev().word)) {
                // have type
                auto type = peek_prev();
                auto ident = advance();
                if (match_it(acc::GLOBAL_TOKENS::TK_EQUALS)) {
                    auto val = parse_expr();
                    return new acc::node::DeclarationStmt{.type = type,
                                                          .name = ident,
                                                          .expr = val};
                } else {
                    throw std::runtime_error("missing '=' for declaration");
                }
            }
        }
        return parse_expression_statement();
        // REMOVE LATER this is just to stop stupid  [-Werror=return-type]
    }

    acc::StmtVariant parse_stmt() {
        return parse_declaration();
    };

   public:
    acc_parser(const std::vector<acc::token>& toks)
        : acc::fsm_storage<std::vector<acc::token>>(toks) {

          };
    void print_ast() {
        acc::printer printer(stmts);
        printer.print();
    };

    std::vector<acc::StmtVariant> parse() {
        do {
            stmts.push_back(parse_stmt());
        } while (!this->is_end());

        return stmts;
    };
    acc_parser(const acc_parser&) = delete;
    acc_parser& operator=(const acc_parser&) = delete;
    acc_parser(acc_parser&&) = default;
    acc_parser& operator=(acc_parser&&) = default;
    ~acc_parser() = default;
};
}  // namespace acc