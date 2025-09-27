#pragma once
#include <array>
#include <iostream>
#include <stack>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../traits/acc_matchable.hpp"
#include "../traits/token_trait.hpp"
#include "../utils/visitor.hpp"
#include "accast.hpp"
#include "accenv.hpp"
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

    std::unordered_map<std::string, acc::node::DeclarationStmt*> m_symbols;
    acc::environment<std::string, acc::node::DeclarationStmt*>* m_env;

    template <traits::acc_matchable T>
    bool check_it(const T& val) const noexcept {
        if (this->is_end()) {
            return false;
        }
        if constexpr (std::is_same_v<T, acc::GLOBAL_TOKENS>) {
            return this->peek().type == val;
        } else {
            return this->peek().word == val;
        }
    }

    template <traits::acc_matchable T>
    bool match_it(const T& val) noexcept {
        if (check_it(val)) {
            DISCARD(this->advance());
            return true;
        }
        return false;
    }

    template <traits::acc_matchable... Ts>
    bool match_any(Ts&&... types) {
        return (match_it(types) || ...);
    }

    template <traits::acc_matchable... Ts>
    bool match_seq(Ts&&... types) {
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

    acc::ExprVariant parse_assignment() {
        while (match_it(acc::GLOBAL_TOKENS::TK_IDENTIFIER)) {
            const auto id_tok = this->peek_prev();
            const std::string id = this->peek_prev().word;
            if (match_it(acc::GLOBAL_TOKENS::TK_EQUALS)) {
                if (acc::node::DeclarationStmt::has_const(this->m_env->get(id)->cv_qual_flags)) {
                    throw parser_error(id_tok, "assignment to const-qualified lvalue");
                }
                auto expr = parse_expr();

                m_env->get(id)->expr = expr;
                m_env->get(id)->history.push_back(expr);

                return expr;
            }
        }

        return parse_comparison();
    }

    acc::ExprVariant parse_expr() {
        return parse_assignment();
    };

    acc::StmtVariant parse_expression_statement() {
        return new acc::node::ExpressionStmt{.expr = parse_expr()};
    }

    acc::StmtVariant parse_variable() {
        // have type
        auto type = peek_prev();
        auto ident = advance();
        std::byte cv_sig{0};
        if (match_it(acc::GLOBAL_TOKENS::TK_COLON)) {
            while (match_it("const") || match_it("volatile")) {
                if (this->peek_prev().word == "const") {
                    if (acc::node::DeclarationStmt::has_const(cv_sig)) {
                        throw acc::parser_error(this->peek_prev(), "duplicate const qualifier");
                    }
                    (cv_sig |= acc::node::DeclarationStmt::mask_const());
                }
                if (this->peek_prev().word == "volatile") {
                    if (acc::node::DeclarationStmt::has_volatile(cv_sig)) {
                        throw acc::parser_error(this->peek_prev(), "duplicate volatile qualifier");
                    }
                    (cv_sig |= acc::node::DeclarationStmt::mask_volatile());
                }
            }
        }

        auto expr = (match_it(acc::GLOBAL_TOKENS::TK_EQUALS) ? std::optional<acc::ExprVariant>(parse_expr())
                                                             : std::optional<acc::ExprVariant>(std::nullopt));
        auto* decl = new acc::node::DeclarationStmt{.type = type,
                                                    .name = ident,
                                                    .cv_qual_flags = cv_sig,
                                                    .history = {},
                                                    .expr = expr};

        m_env->set(decl->name.word, decl);

        /* DEBUG ONLY */
        if (expr.has_value()) {
            if (auto* ptr = m_env->resolve(decl->name.word)) {
                std::cout << "HGEREREREEREREARRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR";
                ptr->get(decl->name.word)->history.push_back(expr.value());
            }
        }
        return decl;
    }

    acc::StmtVariant parse_block() {
        if (match_it(acc::GLOBAL_TOKENS::TK_CURL_R)) {
        }
        std::unreachable();
    };

    // int a : const = 2;
    acc::StmtVariant parse_declaration() {
        if (match_it(acc::GLOBAL_TOKENS::TK_RESERVED)) {
            // check if the reserved word is a type
            if (acc::globals::ACC_TYPE_SET.contains(this->peek_prev().word)) {
                return parse_variable();
            }
        }
        if (match_it(acc::GLOBAL_TOKENS::TK_CURL_L)) {
            return parse_block();
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
        m_env = new acc::environment<std::string, acc::node::DeclarationStmt*>();
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