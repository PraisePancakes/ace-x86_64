#pragma once
#include <array>
#include <iostream>
#include <sstream>
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
#include "statics/ro_acckw.hpp"
#include "statics/ro_accprec.hpp"
#include "storage.hpp"

#define DISCARD(f) (void)f
namespace acc {

class [[nodiscard]] acc_parser
    : public acc::fsm_storage<std::vector<acc::token>> {
    acc::environment<std::string, acc::node::DeclarationStmt*, acc::StmtVariant>* m_env;

    template <traits::acc_matchable T>
    [[nodiscard]] bool check_it(T&& val) const noexcept {
        if (this->is_end()) {
            return false;
        }
        if constexpr (std::is_same_v<T, acc::GLOBAL_TOKENS>) {
            return this->peek().type == val;
        } else {
            return this->peek().word == val;
        }
    }

    [[nodiscard]] bool check_it(traits::acc_matchable auto&&... val) const noexcept {
        return (this->check_it(std::forward<decltype(val)>(val)) || ...);
    }

    template <traits::acc_matchable T>
    bool match_it(T&& val) noexcept {
        if (check_it(std::move(val))) {
            DISCARD(this->advance());
            return true;
        }
        return false;
    }

    template <traits::acc_matchable... Ts>
    bool match_any(Ts&&... types) {
        return (match_it(std::forward<Ts>(types)) || ...);
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
            if (!match_it(TK_PAREN_R)) throw acc::parser_error(this->peek(), "missing ')'");
            return new acc::node::GroupingExpr{.expr = expr};
        }

        throw acc::parser_error(this->peek(), " unknown primary literal ");
    }

    acc::ExprVariant parse_unary() {
        if (match_any(TK_BANG, TK_STAR)) {
            auto op = this->peek_prev();
            auto expr = parse_expr();
            return new acc::node::UnaryExpr{.op = op, .expr = expr};
        }
        return parse_primary();
    };

    acc::ExprVariant parse_id_expression() {
        while (match_it(acc::GLOBAL_TOKENS::TK_IDENTIFIER)) {
            const auto id_tok = this->peek_prev();
            const std::string id = this->peek_prev().word;

            if (!this->m_env->resolve(id)) throw parser_error(id_tok, " unresolved identifier at ");

            if (match_it(acc::GLOBAL_TOKENS::TK_EQUALS)) {
                if (acc::node::DeclarationStmt::has_const(this->m_env->get(id)->cv_qual_flags)) {
                    throw parser_error(id_tok, " assignment to const-qualified lvalue with ");
                }
                auto expr = parse_expr();

                m_env->get(id)->expr = expr;
                m_env->get(id)->history.push_back(expr);

                return expr;
            }

            if (m_env->get(peek_prev().word)->expr.has_value())
                return m_env->get(peek_prev().word)->expr.value();
            throw acc::parser_error(this->peek_prev(), "unknown unqualified-id at ");
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
        auto lhs_atom = parse_id_expression();

        while (true) {
            auto op = peek();

            if (!is_binary_op(op) || globals::prec_map.at(op.type) < min_prec) {
                break;
            }

            auto prec = globals::prec_map.at(op.type);
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

    std::byte get_cv_sig() {
        std::byte cv_sig{1};
        if (match_it(acc::GLOBAL_TOKENS::TK_COLON)) {
            while (match_it("mut") || match_it("volatile")) {
                if (this->peek_prev().word == "mut") {
                    if (!acc::node::DeclarationStmt::has_const(cv_sig)) {
                        throw acc::parser_error(this->peek_prev(), "duplicate mut qualifier");
                    }
                    (cv_sig = ((cv_sig) & (~std::byte{1})));
                }
                if (this->peek_prev().word == "volatile") {
                    if (acc::node::DeclarationStmt::has_volatile(cv_sig)) {
                        throw acc::parser_error(this->peek_prev(), "duplicate volatile qualifier");
                    }
                    (cv_sig |= acc::node::DeclarationStmt::mask_volatile());
                }
            }
        }
        return cv_sig;
    };

    // int x : mut = 4;
    acc::StmtVariant parse_variable_declaration() {
        // have type

        acc::node::DeclarationStmt* decl = new acc::node::DeclarationStmt{.type = peek_prev(),
                                                                          .name = advance(),
                                                                          .cv_qual_flags = get_cv_sig(),
                                                                          .history = {},
                                                                          .expr = (match_it(acc::GLOBAL_TOKENS::TK_EQUALS)
                                                                                       ? std::optional<acc::ExprVariant>(parse_expr())
                                                                                       : std::optional<acc::ExprVariant>(std::nullopt))};

        /* DEBUG INFO */
        if (m_env->resolve(decl->name.word) == m_env) {
            throw acc::parser_error(decl->name, "scope resolved an ambiguous identifier ");
        }

        m_env->set(decl->name.word, decl);
        if (auto* ptr = m_env->resolve(decl->name.word)) {
            if (decl->expr.has_value()) {
                ptr->get(decl->name.word)->history.push_back(decl->expr.value());
            }
        }
        /* DEBUG INFO */

        return decl;
    }

    acc::StmtVariant
    parse_block() {
        acc::node::BlockStmt* block = new acc::node::BlockStmt{.stmts = parse()};
        if (!match_it(acc::GLOBAL_TOKENS::TK_CURL_R)) {
            throw acc::parser_error(this->peek_prev(), " open block without closing '}' with ");
        }

        m_env = m_env->get_parent();
        return block;
    };

    acc::StmtVariant parse_conditional() {
        // if(true) { return false; };
        //  if , ternary?!?!?!?
        return new acc::node::IfStmt{.condition = parse_expr(),
                                     .then = parse_declaration(),
                                     .else_ = (match_it("else") ? std::optional<StmtVariant>(parse_declaration())
                                                                : std::optional<StmtVariant>(std::nullopt))};
    };

    acc::StmtVariant parse_iteration() {
        // for and whiles
        if (peek_prev().word == "while") {
            return new acc::node::WhileStmt{.condition = [this]() -> acc::StmtVariant {
                                                if (!match_it(TK_PAREN_L)) throw acc::parser_error(peek(), " missing opening '(' for while condition ");
                                                auto cond = parse_declaration();
                                                if (!match_it(TK_PAREN_R)) throw acc::parser_error(peek(), " missing closing ')' for while condition ");
                                                return cond;
                                            }(),
                                            .body = parse_declaration()};
        }

        if (peek_prev().word == "for") {
            return new acc::node::ForStmt{.init = [this]() -> acc::StmtVariant {
                                              if (!match_it(TK_PAREN_L))
                                                  throw acc::parser_error(peek(), " missing opening '(' for 'for' conditional list ");
                                              return parse_stmt();
                                          }(),
                                          .condition = parse_stmt(),
                                          .expr = [this]() -> acc::ExprVariant {
                                              auto expr = parse_expr();
                                              if (!match_it(TK_PAREN_R)) throw acc::parser_error(peek(), " missing closing ')' for 'for' conditional list ");
                                              return expr;
                                          }(),
                                          .body = parse_declaration()};
        }
        throw acc::parser_error(peek_prev(), " error parsing iteration statement ");
    };

    acc::StmtVariant parse_function_declaration() {
        acc::node::FuncStmt* func = new acc::node::FuncStmt{
            .type = peek_prev(),
            .name = advance(),
            .params = [this]() -> std::vector<acc::StmtVariant> {
                std::vector<acc::StmtVariant> ret;
                if (match_it(TK_PAREN_L)) {
                    while (!match_it(TK_PAREN_R)) {
                        DISCARD(advance());
                        ret.push_back(parse_variable_declaration());
                        if (!match_it(TK_COMMA) && peek().type != TK_PAREN_R) throw acc::parser_error(peek(), "missing param seperator ',' ");
                    }
                }
                return ret;
            }(),
            .body = parse_declaration()};

        return func;
    }

    // either pass token or embedded string

    // int a : mut = 2;
    acc::StmtVariant parse_declaration() {
        // check if the reserved word is a type or any other reserved
        if (peek().type == acc::GLOBAL_TOKENS::TK_RESERVED) {
            if (match_it("if")) {
                return parse_conditional();
            }
            if (match_any("while", "for")) {
                return parse_iteration();
            }
        }
        if (match_it(acc::GLOBAL_TOKENS::TK_RESERVED_TYPE)) {
            if (peek_next().type == TK_PAREN_L) {
                return parse_function_declaration();
            }
            return parse_variable_declaration();
        }

        if (match_it(acc::GLOBAL_TOKENS::TK_CURL_L)) {
            return parse_block();
        }
        return parse_expression_statement();
    }

    acc::StmtVariant parse_stmt() {
        auto node = parse_declaration();

        if (!match_it(acc::GLOBAL_TOKENS::TK_SEMI)) {
            throw acc::parser_error(this->peek_prev(), "missing ';' in statement ");
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

    void panic() {
        while (!check_it(TK_CURL_R, TK_SEMI) && !this->is_end()) {
            DISCARD(advance());
        }
        DISCARD(advance());
    };

    std::vector<acc::StmtVariant> parse() {
        auto* new_env = new acc::environment<std::string, acc::node::DeclarationStmt*, acc::StmtVariant>();
        new_env->set_parent(m_env);
        m_env = new_env;

        while (!this->is_end() && !check_it(acc::GLOBAL_TOKENS::TK_CURL_R)) {
            try {
                m_env->get_items().push_back(parse_stmt());
            } catch (acc::parser_error const& err) {
                report_error(err);
                panic();
            };
        };

        return new_env->get_items();
    };
    acc_parser(const acc_parser&) = delete;
    acc_parser& operator=(const acc_parser&) = delete;
    acc_parser(acc_parser&&) = default;
    acc_parser& operator=(acc_parser&&) = default;
    ~acc_parser() = default;
};
}  // namespace acc