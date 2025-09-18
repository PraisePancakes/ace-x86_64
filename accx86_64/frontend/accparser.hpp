#pragma once
#include <array>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../traits/token_trait.hpp"
#include "../utils/visitor.hpp"
#include "accast.hpp"
#include "accparserror.hpp"
#include "accprinter.hpp"
#include "acctoken.hpp"
#include "statics/ro_acctypes.hpp"
#include "storage.hpp"

#define DISCARD(f) (void)f
namespace acc {

class [[nodiscard]] acc_parser
    : public acc::fsm_storage<std::vector<acc::token>> {
    std::vector<acc::StmtVariant> stmts;
    std::unordered_map<std::string, const acc::node::DeclarationStmt*> m_symbols;

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
            if (!match_it(TK_PAREN_R)) throw acc::parser_error(this->peek_prev(), "missing ')'");
            return new acc::node::GroupingExpr{.expr = expr};
        }

        throw acc::parser_error(this->peek_prev(), "unknown primary literal");
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

    acc::StmtVariant parse_variable() {
        if (acc::globals::ACC_TYPE_SET.contains(this->peek_prev().word)) {
            // have type
            auto type = peek_prev();
            auto ident = advance();
            std::byte cv_sig{0};
            if (match_it(acc::GLOBAL_TOKENS::TK_COLON)) {
                while (this->peek().word == "const" || this->peek().word == "volatile") {
                    if (this->peek().word == "const") {
                        if (acc::node::DeclarationStmt::has_const(cv_sig)) {
                            throw acc::parser_error(this->peek_prev(), "duplicate const qualifier");
                        }
                        (cv_sig |= acc::node::DeclarationStmt::mask_const());
                    }
                    if (this->peek().word == "volatile") {
                        if (acc::node::DeclarationStmt::has_volatile(cv_sig)) {
                            throw acc::parser_error(this->peek_prev(), "duplicate volatile qualifier");
                        }
                        (cv_sig |= acc::node::DeclarationStmt::mask_volatile());
                    }
                    DISCARD(advance());
                }
            }

            acc::node::DeclarationStmt* decl = new acc::node::DeclarationStmt{.type = type,
                                                                              .name = ident,
                                                                              .cv_qual_flags = cv_sig,
                                                                              .expr = (match_it(acc::GLOBAL_TOKENS::TK_EQUALS) ? std::optional<acc::ExprVariant>(parse_expr())
                                                                                                                               : std::optional<acc::ExprVariant>(std::nullopt))};
            m_symbols[decl->name.word] = decl;
            return decl;
        }
        std::unreachable();
    }
    // int a : const = 2;
    acc::StmtVariant parse_declaration() {
        if (match_it(acc::GLOBAL_TOKENS::TK_RESERVED)) {
            return parse_variable();
        }
        return parse_expression_statement();
    }

    acc::StmtVariant parse_stmt() {
        auto node = parse_declaration();
        if (!match_it(acc::GLOBAL_TOKENS::TK_SEMI)) {
            throw acc::parser_error(this->peek_prev(), "missing ';' in statement");
        }
        return node;
    };

    static void report_error(const acc::parser_error& error) noexcept {
        std::stringstream msg;
        msg << error.what
            << " at ("
            << error.token.location.first
            << " , "
            << error.token.location.second
            << ")\n";

        acc::logger::instance().send(acc::logger::LEVEL::ERROR, msg.str());
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
        try {
            do {
                stmts.push_back(parse_stmt());
            } while (!this->is_end());
        } catch (acc::parser_error const& err) {
            report_error(err);
            // panic recover
        };

        return stmts;
    };
    acc_parser(const acc_parser&) = delete;
    acc_parser& operator=(const acc_parser&) = delete;
    acc_parser(acc_parser&&) = default;
    acc_parser& operator=(acc_parser&&) = default;
    ~acc_parser() = default;
};
}  // namespace acc