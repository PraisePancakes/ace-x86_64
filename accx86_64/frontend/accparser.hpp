#pragma once
#include <array>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../traits/acc_matchable.hpp"
#include "../traits/token_trait.hpp"
#include "../utils/acclog.hpp"
#include "../utils/visitor.hpp"
#include "accast.hpp"
#include "accenv.hpp"
#include "accparserror.hpp"
#include "acctoken.hpp"
#include "analysis/analyzer.hpp"
#include "printers/ast_printer.hpp"
#include "statics/ro_accprec.hpp"
#include "storage.hpp"

#define DISCARD(f) (void)f
namespace acc {

class [[nodiscard]] acc_parser
    : public acc::fsm_storage<std::vector<acc::token>> {
    using environment_t = acc::environment<std::string, acc::StmtVariant>;
    environment_t* m_env{nullptr};
    bool in_params = false;

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

    acc::ExprVariant parse_primary_expression() {
        if (match_any(TK_LITERAL_INT,
                      TK_LITERAL_STRING,
                      TK_LITERAL_DOUBLE,
                      TK_LITERAL_FLOAT)) {
            return new acc::node::LiteralExpr{.value = this->peek_prev().value,
                                              .embedded = this->peek_prev()};
        };

        if (match_it(TK_PAREN_L)) {
            in_params = false;
            auto expr = parse_expr();
            if (!match_it(TK_PAREN_R)) throw acc::exceptions::parser_error(this->peek(), "missing ')'");
            return new acc::node::GroupingExpr{.expr = expr};
        }

        return std::monostate{};
    }

    acc::ExprVariant parse_unary_expression() {
        if (match_any(TK_BANG, TK_STAR)) {
            auto op = this->peek_prev();
            auto expr = parse_expr();

            return new acc::node::UnaryExpr{.op = op, .expr = expr};
        }
        return parse_primary_expression();
    };

    acc::ExprVariant parse_variable_expression() const {
        const auto id_tok = this->peek_prev();
        return new acc::node::VariableExpr{.type = m_env->get<acc::node::DeclarationStmt*>(id_tok.word)->type, .name = id_tok, .deduced_value = std::monostate{}};
    };

    // anything to do with an identifier in an expression e.g a = 4 or a() or anything gets resolved here
    acc::ExprVariant parse_id_expression() {
        while (match_it(TK_IDENTIFIER)) {
            const auto id_tok = this->peek_prev();
            const std::string id = this->peek_prev().word;

            if (!this->m_env->resolve(id)) throw exceptions::parser_error(id_tok, " unresolved identifier at ");

            if (match_it(TK_ASSIGNMENT)) {
                if (acc::node::DeclarationStmt::has_const(this->m_env->get<acc::node::DeclarationStmt*>(id)->cv_qual_flags)) {
                    throw exceptions::parser_error(id_tok, " assignment to const-qualified lvalue with ");
                }
                auto expr = parse_expr();
                auto* original = m_env->get<acc::node::DeclarationStmt*>(id);
                original->expr = expr;
                original->history.push_back(expr);

                return new acc::node::AssignmentExpr{.type = original->type, .name = id_tok, .expr = expr};
            }

            if (match_it(TK_EQUALS)) {
                throw exceptions::parser_error(id_tok, " declaration operator used in place of assignment replace '=' with ':=' ");
            };

            /*
            // id's that arent a function call being used to evaluate an expression
            ie : a + 4 or 4 * 3 - z and which its value can be deduced in an evaluation context.
            Hence why we get the expr.value() to return.
            */
            if (m_env->get<acc::node::DeclarationStmt*>(id)->expr.has_value())
                return m_env->get<acc::node::DeclarationStmt*>(id)->expr.value();

            // check if function call here to evaluate else return parse_variable_expression();
            if (match_it(TK_PAREN_L)) {
                try {
                    in_params = true;
                    auto* func_info = m_env->get<acc::node::FuncStmt*>(id);
                    std::vector<ExprVariant> args;
                    while (!match_it(TK_PAREN_R)) {
                        args.push_back(parse_expr());
                    }
                    if (args.size() != func_info->params.size()) {
                        throw exceptions::parser_error(id_tok, "wrong number of arguments ( " + std::to_string(args.size()) + ", should be " + std::to_string(func_info->params.size()));
                    }
                    return new acc::node::CallExpr{.args = args, .procedure = func_info};

                } catch (std::runtime_error& err) {
                    throw exceptions::parser_error(id_tok, " failed to resolve function with id ");
                };
            };
            /*
             id's in which its value cannot be deduced because its in an unevaluated context such as a function definition
             e.g: int f(int h, int y) { int z = h + y; };
             we dont know what h and y are therefore we just make them variables to be deduced at call site.
            */
            return parse_variable_expression();
        }

        return parse_unary_expression();
    }

    // 1  *  2 + 3
    acc::ExprVariant parse_expr(std::size_t min_prec = 0) {
        auto lhs_atom = parse_id_expression();
        while (true) {
            auto op = peek();

            if (!acc::globals::lexeme_inspector::is_binary_op(op, in_params) || globals::prec_map.at(op.type) < min_prec) {
                break;
            }

            auto prec = globals::prec_map.at(op.type);
            auto next_min_prec = prec + 1;
            DISCARD(advance());
            auto rhs_atom = parse_expr(next_min_prec);
            lhs_atom = new acc::node::BinaryExpr{.lhs = lhs_atom, .rhs = rhs_atom, .op = op};
        }
        return lhs_atom;
    };

    acc::StmtVariant parse_expression_statement() {
        auto* expr = new acc::node::ExpressionStmt{.expr = parse_expr()};
        if (!match_it(TK_SEMI)) throw acc::exceptions::parser_error(peek_prev(), "missing semi ';' ");
        return expr;
    }

    void report_error(const acc::exceptions::parser_error& error) const noexcept {
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

    void panic() {
        while (!check_it(TK_CURL_R, TK_SEMI) && !this->is_end()) {
            DISCARD(advance());
        }
        DISCARD(advance());
    };

    auto* create_environment() const {
        auto* env = new environment_t();
        env->set_parent(m_env);
        return env;
    };

    struct ReturnException : std::exception {
        acc::node::ReturnStmt* ret;
        ReturnException(acc::node::ReturnStmt* r) : ret(r) {};
    };

    acc::StmtVariant parse_block() {
        auto* block = new acc::node::BlockStmt();
        auto* new_env = create_environment();
        try {
            block->env = parse(new_env);
        } catch (const ReturnException& r) {
            block->ret = r.ret;
            new_env->get_items().push_back(r.ret);
        }
        block->env = new_env;
        match_it(TK_SEMI);

        return block;
    };

    std::byte get_cv_sig() {
        std::byte cv_sig{1};
        if (match_it(acc::GLOBAL_TOKENS::TK_COLON)) {
            while (match_any("mut", "volatile")) {
                if (this->peek_prev().word == "mut") {
                    if (!acc::node::DeclarationStmt::has_const(cv_sig)) {
                        throw acc::exceptions::parser_error(this->peek_prev(), "duplicate mut qualifier");
                    }
                    acc::node::DeclarationStmt::remove_const(cv_sig);
                }
                if (this->peek_prev().word == "volatile") {
                    if (acc::node::DeclarationStmt::has_volatile(cv_sig)) {
                        throw acc::exceptions::parser_error(this->peek_prev(), "duplicate volatile qualifier");
                    }
                    acc::node::DeclarationStmt::set_volatile(cv_sig);
                }
            }
        }
        return cv_sig;
    };

    acc::StmtVariant parse_variable_declaration() {
        acc::node::DeclarationStmt* decl = new acc::node::DeclarationStmt{.type = peek_prev(),
                                                                          .name = advance(),
                                                                          .cv_qual_flags = get_cv_sig(),
                                                                          .history = {},
                                                                          .expr = (match_it(acc::GLOBAL_TOKENS::TK_EQUALS)
                                                                                       ? std::optional<acc::ExprVariant>(parse_expr())
                                                                                       : std::optional<acc::ExprVariant>(std::nullopt))};
        if (!match_it(TK_SEMI) && !in_params) throw acc::exceptions::parser_error(peek_prev(), "Missing semi ';' ");

        /* DEBUG INFO */
        if (m_env->resolve(decl->name.word) == m_env) {
            throw acc::exceptions::parser_error(decl->name, "scope resolved an ambiguous identifier ");
        }

        m_env->set(decl->name.word, decl);
        if (auto* ptr = m_env->resolve(decl->name.word)) {
            if (decl->expr.has_value()) {
                ptr->get<acc::node::DeclarationStmt*>(decl->name.word)->history.push_back(decl->expr.value());
            }
        }
        /* DEBUG INFO */

        return decl;
    };

    acc::StmtVariant parse_conditional() {
        return new acc::node::IfStmt{.condition = parse_expr(),
                                     .then = parse_statement(),
                                     .else_ = (match_it("else") ? std::optional<StmtVariant>(parse_statement())
                                                                : std::optional<StmtVariant>(std::nullopt))};
    };
    acc::StmtVariant parse_iteration() {
        if (peek_prev().word == "while") {
            return new acc::node::WhileStmt{.condition = [this]() -> acc::ExprVariant {
                                                if (!match_it(TK_PAREN_L)) throw acc::exceptions::parser_error(peek(), " missing opening '(' for while condition ");
                                                auto cond = parse_expr();
                                                if (!match_it(TK_PAREN_R)) throw acc::exceptions::parser_error(peek(), " missing closing ')' for while condition ");
                                                return cond;
                                            }(),
                                            .body = parse_statement()};
        }

        if (peek_prev().word == "for") {
            return new acc::node::ForStmt{.init = [this]() -> acc::StmtVariant {
                                              if (!match_it(TK_PAREN_L))
                                                  throw acc::exceptions::parser_error(peek(), " missing opening '(' for 'for' conditional list ");
                                              return parse_identifier_statement();
                                          }(),
                                          .condition = parse_expression_statement(),
                                          .expr = [this]() -> acc::ExprVariant {
                                              auto expr = parse_expr();
                                              if (!match_it(TK_PAREN_R)) throw acc::exceptions::parser_error(peek(), " missing closing ')' for 'for' conditional list ");
                                              return expr;
                                          }(),
                                          .body = parse_statement()};
        }
        std::unreachable();
    };

    acc::StmtVariant parse_function_declaration() {
        return new acc::node::FuncStmt{.type = peek_prev(),
                                       .name = advance(),
                                       .params = [this]() -> std::vector<acc::node::DeclarationStmt*> {
                                           std::vector<acc::node::DeclarationStmt*> params;
                                           if (match_it(TK_PAREN_L)) {
                                               this->in_params = true;
                                               while (!match_it(TK_PAREN_R)) {
                                                   DISCARD(advance());
                                                   params.push_back(std::get<acc::node::DeclarationStmt*>(this->parse_variable_declaration()));
                                                   if (!match_it(TK_COMMA) && peek().type != TK_PAREN_R)
                                                       throw acc::exceptions::parser_error(peek(), "missing param seperator ',' ");
                                               }
                                           }
                                           return params;
                                       }(),
                                       .body = std::get<acc::node::BlockStmt*>(parse_statement())};
    };

    acc::StmtVariant parse_identifier_statement() {
        if (match_it(TK_RESERVED_TYPE)) {
            if (peek_next().type == TK_PAREN_L) {
                // parse function
                return parse_function_declaration();
            }

            // parse var
            return parse_variable_declaration();
        }

        return parse_expression_statement();
    }

    acc::StmtVariant parse_statement() {
        if (match_it(TK_CURL_L)) {
            return parse_block();
        }

        // if, while, for
        if (check_it(TK_RESERVED)) {
            if (match_it("if")) {
                return parse_conditional();
            }
            if (match_any("while", "for")) {
                return parse_iteration();
            }

            if (match_it("return")) {
                auto* ret = new acc::node::ReturnStmt{.keyword = this->peek_prev(), .expr = parse_expr()};
                if (!match_it(TK_SEMI)) {
                    throw acc::exceptions::parser_error(peek_prev(), "Missing semi ';' ");
                }
                throw ReturnException{ret};
            }
        }
        return parse_identifier_statement();
    };

   public:
    acc_parser(const std::vector<acc::token>& toks)
        : acc::fsm_storage<std::vector<acc::token>>(toks), m_env{nullptr} {

          };

    acc::environment<std::string, acc::StmtVariant>* parse(acc::environment<std::string, acc::StmtVariant>* env) {
        m_env = env;
        while (!is_end() && !match_it(TK_CURL_R)) {
            try {
                env->get_items().push_back(parse_statement());
            } catch (acc::exceptions::parser_error const& err) {
                report_error(err);
                panic();
            };
        };
        m_env = env->get_parent();
        return env;
    };

    acc::environment<std::string, acc::StmtVariant>* parse() {
        auto* new_env = create_environment();
        m_env = new_env;
        while (!is_end() && !match_it(TK_CURL_R)) {
            try {
                new_env->get_items().push_back(parse_statement());
            } catch (acc::exceptions::parser_error const& err) {
                report_error(err);
                panic();
            };
        };
        m_env = new_env->get_parent();

        return new_env;
    };

    acc_parser(const acc_parser&) = delete;
    acc_parser& operator=(const acc_parser&) = delete;
    acc_parser(acc_parser&&) = default;
    acc_parser& operator=(acc_parser&&) = default;
    ~acc_parser() = default;
};
}  // namespace acc