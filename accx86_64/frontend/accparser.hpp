#pragma once
#include <array>
#include <iostream>
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
    acc::environment<std::string, acc::node::DeclarationStmt*, acc::StmtVariant>* m_env;
    inline static const std::unordered_map<acc::GLOBAL_TOKENS, std::size_t> prec_map{
        {acc::TK_COMMA, 0},
        {acc::TK_EQUALS, 1},
        {acc::TK_LT, 2},
        {acc::TK_GT, 2},
        {acc::TK_GT_EQ, 2},
        {acc::TK_LT_EQ, 2},
        {acc::TK_PLUS, 3},
        {acc::TK_DASH, 3},
        {acc::TK_SLASH, 4},
        {acc::TK_STAR, 4},
       
        };

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

    acc::ExprVariant parse_assignment() {
        while (match_it(acc::GLOBAL_TOKENS::TK_IDENTIFIER)) {
            const auto id_tok = this->peek_prev();
            const std::string id = this->peek_prev().word;
            try {
                if (match_it(acc::GLOBAL_TOKENS::TK_EQUALS)) {
                    if (acc::node::DeclarationStmt::has_const(this->m_env->get(id)->cv_qual_flags)) {
                        throw parser_error(id_tok, "assignment to const-qualified lvalue");
                    }
                    auto expr = parse_expr();

                    m_env->get(id)->expr = expr;
                    m_env->get(id)->history.push_back(expr);

                    return expr;
                }
            } catch (std::exception& e) {
                throw acc::parser_error(id_tok, " unresolved symbol ");
            };
        }

        return parse_unary();
    }

    bool is_binary_op(acc::token tok) {
        switch (tok.type) {
            case TK_PLUS:
            case TK_DASH:
            case TK_SLASH:
            case TK_STAR:
            case TK_COMMA:
            case TK_LT:
            case TK_GT:
            case TK_LT_EQ:
            case TK_GT_EQ:
            case TK_EQUALS:
                return true;
            default:
                return false;
        }
    }
 // 1  *  2 + 3
    acc::ExprVariant parse_expr_prec(std::size_t min_prec) {
        auto lhs_atom = parse_assignment();
        while (true) {
            auto op = peek();
            if (!is_binary_op(op) || prec_map.at(op.type) < min_prec) {
                break;
            }

            auto prec = prec_map.at(op.type);
            auto next_min_prec = prec + 1;
            DISCARD(advance());
            auto rhs_atom = parse_expr_prec(next_min_prec);
            lhs_atom = new acc::node::BinaryExpr{.lhs = lhs_atom, .rhs = rhs_atom, .op = op};
        }
        return lhs_atom;
    };

    acc::ExprVariant parse_expr() {
        return parse_expr_prec(0);
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
        if (m_env->resolve(decl->name.word) == m_env) {
            throw acc::parser_error(decl->name, "scope resolved an ambiguous identifier ");
        }
        m_env->set(decl->name.word, decl);

        /* DEBUG ONLY */
        if (expr.has_value()) {
            if (auto* ptr = m_env->resolve(decl->name.word)) {
                ptr->get(decl->name.word)->history.push_back(expr.value());
            }
        }
        return decl;
    }

    acc::StmtVariant parse_block() {
        acc::node::BlockStmt* block = new acc::node::BlockStmt{.stmts = parse()};

        if (!match_it(acc::GLOBAL_TOKENS::TK_CURL_R)) {
            throw acc::parser_error(this->peek_prev(), "open block without closing '}'");
        }

        return block;
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
            << "'" << error.token.word << "'"
            << " at ("
            << error.token.location.first
            << " , "
            << error.token.location.second
            << ")\n";

        acc::logger::instance().send(acc::logger::LEVEL::ERROR, msg.str());
    };

   public:
    acc_parser(const std::vector<acc::token>& toks)
        : acc::fsm_storage<std::vector<acc::token>>(toks), m_env{nullptr} {

          };
    void print_ast() {
        acc::printer printer(m_env->get_root()->get_items());
        printer.print();
    };

    std::vector<acc::StmtVariant> parse() {
        auto* new_env = new acc::environment<std::string, acc::node::DeclarationStmt*, acc::StmtVariant>();
        new_env->set_parent(m_env);
        m_env = new_env;

        try {
            do {
                m_env->get_items().push_back(parse_stmt());
            } while (!this->is_end() && !check_it(acc::GLOBAL_TOKENS::TK_CURL_R));
        } catch (acc::parser_error const& err) {
            report_error(err);
            // panic recover
        };

        return m_env->get_items();
    };
    acc_parser(const acc_parser&) = delete;
    acc_parser& operator=(const acc_parser&) = delete;
    acc_parser(acc_parser&&) = default;
    acc_parser& operator=(acc_parser&&) = default;
    ~acc_parser() = default;
};
}  // namespace acc