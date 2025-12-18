#pragma once
#include <iostream>
#include <vector>

#include "../../utils/inker.hpp"
#include "../accast.hpp"
#include "token_printer.hpp"

namespace acc::output {
class ast_printer {
    std::size_t m_depth = 0;
    std::vector<acc::StmtVariant> stmts;

   public:
    void print_expression(const acc::ExprVariant expr, std::ostream& os) {
        // non owning view of nodes
        acc::utils::inker inker(os);
        std::visit(acc::internal::visitor{[this, &inker](const acc::node::LiteralExpr* expr) {
                                              inker.print_depth(m_depth, ' ').print_yellow("( LITERAL )");
                                              inker.print_depth(m_depth, ' ').print_yellow("|__");
                                              m_depth += 5;
                                              inker.print_depth(m_depth, ' ').print_blue(expr->embedded.word);
                                              m_depth -= 5;
                                          },
                                          [this, &inker](const acc::node::BinaryExpr* expr) {
                                              inker.print_depth(m_depth, ' ').print_yellow("( BINARY )");
                                              inker.print_depth(m_depth, ' ').print_yellow("|__");
                                              m_depth += 5;
                                              print_expression(expr->lhs, inker.os);
                                              inker.print_depth(m_depth, ' ').print_green(expr->op.word);
                                              print_expression(expr->rhs, inker.os);
                                              m_depth -= 5;
                                          },
                                          [this, &inker](const acc::node::UnaryExpr* expr) {
                                              inker.print_depth(m_depth, ' ').print_yellow("( UNARY )");
                                              inker.print_depth(m_depth, ' ').print_yellow("|__");
                                              m_depth += 5;
                                              inker.print_depth(m_depth, ' ').print_green(expr->op.word);
                                              print_expression(expr->expr, inker.os);
                                              m_depth -= 5;
                                          },
                                          [this, &inker](const acc::node::GroupingExpr* expr) {
                                              inker.print_depth(m_depth, ' ').print_yellow("( GROUPING )");
                                              inker.print_depth(m_depth, ' ').print_yellow("|__");
                                              m_depth += 5;
                                              print_expression(expr->expr, inker.os);
                                              m_depth -= 5;
                                          },
                                          [this, &inker](const acc::node::CallExpr* expr) {
                                              inker.print_depth(m_depth, ' ').print_yellow("( Call )");
                                              inker.print_depth(m_depth, ' ').print_yellow("|__");
                                              m_depth += 5;
                                              inker.print_depth(m_depth, ' ').print_green("<PROC>");
                                              print_statement(expr->procedure, inker.os);
                                              inker.print_depth(m_depth, ' ').print_yellow("( ARGS )");
                                              inker.print_depth(m_depth, ' ').print_yellow("|__");
                                              if (expr->args.size())
                                                  for (const auto& a : expr->args) {
                                                      print_expression(a, inker.os);
                                                  }
                                              else {
                                                  inker.print_depth(m_depth, ' ').print_red("NULL");
                                              }

                                              m_depth -= 5;
                                          },
                                          [this, &inker](const acc::node::VariableExpr* expr) {
                                              inker.print_depth(m_depth, ' ').print_yellow("( VARIABLE )");
                                              inker.print_depth(m_depth, ' ').print_yellow("|__");
                                              m_depth += 5;
                                              inker.print_depth(m_depth, ' ').print_green(expr->name.word);
                                              if (expr->evaluated.has_value()) {
                                                  print_expression(expr->evaluated.value(), inker.os);
                                              } else {
                                                  inker.print_depth(m_depth, ' ').print_red("UNEVALUATED");
                                              }
                                              m_depth -= 5;
                                          },
                                          [this, &inker](const acc::node::AssignmentExpr* expr) {
                                              inker.print_depth(m_depth, ' ').print_yellow("( ASSIGNMENT )");
                                              inker.print_depth(m_depth, ' ').print_yellow("|__");
                                              m_depth += 5;
                                              inker.print_depth(m_depth, ' ').print_green(expr->name.word);
                                              inker.print_depth(m_depth, ' ').print_blue(":=");
                                              print_expression(expr->expr, inker.os);
                                              m_depth -= 5;
                                          },
                                          [](std::monostate) { std::unreachable(); }},
                   expr);
    };

    void print_statement(const acc::StmtVariant stmt, std::ostream& os) {
        acc::utils::inker inker(os);
        std::visit(acc::internal::visitor{[this, &inker](const acc::node::IfStmt* stmt) {
                                              inker.print_depth(m_depth, ' ').print_yellow("[ IF ]");
                                              inker.print_depth(m_depth, ' ').print_yellow("|__");
                                              m_depth += 5;
                                              print_expression(stmt->condition, inker.os);
                                              print_statement(stmt->then, inker.os);
                                              if (stmt->else_.has_value()) {
                                                  print_statement(stmt->else_.value(), inker.os);
                                              }
                                              m_depth -= 5;
                                          },
                                          [this, &inker](const acc::node::WhileStmt* stmt) {
                                              inker.print_depth(m_depth, ' ').print_yellow("[ WHILE ]");
                                              inker.print_depth(m_depth, ' ').print_yellow("|__");
                                              m_depth += 5;
                                              print_expression(stmt->condition, inker.os);
                                              print_statement(stmt->body, inker.os);
                                              m_depth -= 5;
                                          },
                                          [this, &inker](const acc::node::BlockStmt* stmt) {
                                              inker.print_depth(m_depth, ' ').print_yellow("[ BLOCK ]");
                                              inker.print_depth(m_depth, ' ').print_yellow("|__");
                                              m_depth += 5;
                                              if (stmt->env) {
                                                  for (const auto& item : stmt->env->get_items()) {
                                                      print_statement(item, inker.os);
                                                  }
                                              }
                                              m_depth -= 5;
                                          },
                                          [this, &inker](const acc::node::ForStmt* stmt) {
                                              inker.print_depth(m_depth, ' ').print_yellow("[ FOR ]");
                                              inker.print_depth(m_depth, ' ').print_yellow("|__");
                                              m_depth += 5;
                                              inker.print_depth(m_depth, ' ').print_yellow("<INIT>");
                                              print_statement(stmt->init, inker.os);
                                              inker.print_depth(m_depth, ' ').print_yellow("<CONDITION>");
                                              print_statement(stmt->condition, inker.os);
                                              inker.print_depth(m_depth, ' ').print_yellow("<EXPRESSION>");
                                              print_expression(stmt->expr, inker.os);
                                              print_statement(stmt->body, inker.os);
                                              m_depth -= 5;
                                          },
                                          [this, &inker](const acc::node::DeclarationStmt* stmt) {
                                              inker.print_depth(m_depth, ' ').print_yellow("[ DECLARATION ]");
                                              inker.print_depth(m_depth, ' ').print_yellow("|__");
                                              m_depth += 5;

                                              if (stmt->access_specifier.has_value()) {
                                                  inker.print_depth(m_depth, ' ').print_blue(stmt->access_specifier.value() ? "private" : "public");
                                              }

                                              if (acc::node::DeclarationStmt::has_const(stmt->cv_qual_flags))
                                                  inker.print_depth(m_depth, ' ').print_blue("const");
                                              else if (acc::node::DeclarationStmt::has_volatile(stmt->cv_qual_flags))
                                                  inker.print_depth(m_depth, ' ').print_blue("volatile");
                                              else
                                                  inker.print_depth(m_depth, ' ').print_blue("mutable");

                                              inker.print_depth(m_depth, ' ').print_green(acc::utils::type_inspector::to_string(stmt->type));

                                              inker.print_depth(m_depth, ' ').print_green(stmt->name.word);

                                              if (stmt->expr.has_value()) {
                                                  inker.print_depth(m_depth, ' ').print_yellow("=");
                                                  print_expression(stmt->expr.value(), inker.os);
                                              }
                                              m_depth -= 5;
                                          },
                                          [this, &inker](const acc::node::ExpressionStmt* stmt) {
                                              inker.print_depth(m_depth, ' ').print_yellow("[ EXPR ]");
                                              inker.print_depth(m_depth, ' ').print_yellow("|__");
                                              m_depth += 5;
                                              print_expression(stmt->expr, inker.os);
                                              m_depth -= 5;
                                          },
                                          [this, &inker](const acc::node::FuncStmt* stmt) {
                                              inker.print_depth(m_depth, ' ').print_yellow("[ FUNCTION ]");
                                              inker.print_depth(m_depth, ' ').print_yellow("|__");
                                              m_depth += 5;

                                              if (stmt->access_specifier.has_value()) {
                                                  inker.print_depth(m_depth, ' ')
                                                      .print_blue(stmt->access_specifier.value() ? "private" : "public");
                                              }

                                              // signature
                                              inker.print_depth(m_depth, ' ').print_green(acc::utils::type_inspector::to_string(stmt->type));
                                              inker.print_depth(m_depth, ' ').print_green(stmt->name.word);

                                              // parameters
                                              inker.print_depth(m_depth, ' ').print_yellow("( PARAMS )");
                                              inker.print_depth(m_depth, ' ').print_yellow("|__");
                                              m_depth += 5;
                                              if (stmt->params.empty()) {
                                                  inker.print_depth(m_depth, ' ').print_red("NULL");
                                              } else {
                                                  for (const auto& p : stmt->params) {
                                                      print_statement(p, inker.os);
                                                  }
                                              }
                                              m_depth -= 5;

                                              // body
                                              if (stmt->body)
                                                  print_statement(stmt->body, inker.os);
                                              else
                                                  inker.print_depth(m_depth, ' ').print_red("NULL");

                                              m_depth -= 5;
                                          },
                                          [this, &inker](const acc::node::ReturnStmt* stmt) {
                                              inker.print_depth(m_depth, ' ').print_yellow("[ RETURN ]");
                                              inker.print_depth(m_depth, ' ').print_yellow("|__");
                                              m_depth += 5;

                                              if (!std::holds_alternative<std::monostate>(stmt->expr)) {
                                                  print_expression(stmt->expr, inker.os);
                                              } else {
                                                  inker.print_depth(m_depth, ' ').print_red("VOID");
                                              }

                                              m_depth -= 5;
                                          },
                                          [this, &inker](const acc::node::TypeStmt* stmt) {
                                              inker.print_depth(m_depth, ' ').print_yellow("[ TYPE ]");
                                              inker.print_depth(m_depth, ' ').print_yellow("|__");
                                              m_depth += 5;

                                              inker.print_depth(m_depth, ' ').print_green(stmt->type_name.word);

                                              for (auto const& m : stmt->members->get_items()) {
                                                  print_statement(m, inker.os);
                                              };

                                              m_depth -= 5;
                                          },
                                          [](std::monostate) { std::unreachable(); }},
                   stmt);
    };
    std::ostream& os;
    ast_printer(std::ostream& os) : os(os) {};
    void dump(const std::vector<acc::StmtVariant>& stmts) {
        for (const auto& s : stmts) {
            print_statement(s, os);
        }
    };
};
}  // namespace acc::output