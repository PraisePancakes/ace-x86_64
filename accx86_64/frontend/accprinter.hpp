#pragma once
#include <iostream>
#include <vector>

#include "../utils/ansi.hpp"
#include "accast.hpp"

namespace acc {
class printer {
    std::size_t m_depth = 0;
    std::vector<acc::StmtVariant> stmts;

   public:
    void print_expression(const acc::ExprVariant expr) {
        // non owning view of nodes
        m_depth++;
        for (std::size_t i = 0; i < m_depth; i++) {
            std::cout << "     ";
        }
        std::visit(internal::visitor{[this](const acc::node::BinaryExpr* bxpr) {
                                         std::cout << acc::ansi::foreground_green << " [ BINARY EXPR ] " << acc::ansi::reset << std::endl;
                                         print_expression(bxpr->lhs);
                                         for (std::size_t i = 0; i < m_depth; i++) {
                                             std::cout << "     ";
                                         }
                                         std::cout << acc::ansi::foreground_light_red << bxpr->op.word << acc::ansi::reset << std::endl;
                                         print_expression(bxpr->rhs);
                                     },
                                     [this](const acc::node::LiteralExpr* lxpr) {
                                         std::cout << acc::ansi::foreground_green << " [ LITERAL EXPR ] " << acc::ansi::reset << std::endl;
                                         for (std::size_t i = 0; i < m_depth; i++) {
                                             std::cout << "     ";
                                         }
                                         std::cout << ansi::foreground_yellow << lxpr->embedded.word << acc::ansi::reset << std::endl;
                                     },
                                     [this](const acc::node::UnaryExpr* uexpr) {
                                         std::cout << acc::ansi::foreground_green << " [ UNARY EXPR ] " << acc::ansi::reset << std::endl;
                                         for (std::size_t i = 0; i < m_depth; i++) {
                                             std::cout << "     ";
                                         }
                                         std::cout << acc::ansi::foreground_light_red << uexpr->op.word << acc::ansi::reset << std::endl;
                                         print_expression(uexpr->expr);
                                     },
                                     [this](const acc::node::GroupingExpr* gexpr) {
                                         std::cout << acc::ansi::foreground_green << " [ GROUP EXPR ] " << acc::ansi::reset << std::endl;

                                         print_expression(gexpr->expr);
                                     },
                                     [this](const acc::node::ComparisonExpr* cxpr) {
                                         std::cout << acc::ansi::foreground_green << " [ COMPARISON EXPR ] " << acc::ansi::reset << std::endl;
                                         print_expression(cxpr->lhs);
                                         for (std::size_t i = 0; i < m_depth; i++) {
                                             std::cout << "     ";
                                         }
                                         std::cout << acc::ansi::foreground_light_red << cxpr->op.word << acc::ansi::reset << std::endl;
                                         print_expression(cxpr->rhs);
                                     }},
                   expr);
        m_depth--;
    };

    void print_statement(const acc::StmtVariant stmt) {
        m_depth++;
        for (std::size_t i = 0; i < m_depth; i++) {
            std::cout << "     ";
        }
        std::visit(internal::visitor{
                       [this](const acc::node::IfStmt* ifstmt) {
                           std::cout << acc::ansi::foreground_green << "\n\n( IF )\n\n"
                                     << acc::ansi::reset << std::endl;
                           std::cout << acc::ansi::foreground_yellow << "CONDITION : " << acc::ansi::reset << std::endl;
                           print_expression(ifstmt->condition);
                           this->print_statement(ifstmt->then);
                           this->print_statement(ifstmt->else_);
                       },
                       [this](const acc::node::DeclarationStmt* declstmt) {
                           std::cout << acc::ansi::foreground_green << "\n\n( DECLARATION )\n\n"
                                     << acc::ansi::reset << std::endl;
                           std::cout << acc::ansi::foreground_yellow << "TYPE : " << acc::ansi::reset << std::endl;
                           declstmt->type.print_token();
                           std::cout << acc::ansi::foreground_yellow << "QUALIFIERS : " << acc::ansi::reset << std::endl;
                           bool has_const = ((declstmt->cv_qual_flags & acc::node::DeclarationStmt::mask_const()) == acc::node::DeclarationStmt::mask_const());
                           bool has_volatile = (((declstmt->cv_qual_flags & acc::node::DeclarationStmt::mask_volatile()) == acc::node::DeclarationStmt::mask_volatile()));
                           std::cout << "[ " << (has_const ? "const" : " ") << " " << (has_volatile ? "volatile" : " ") << " ]" << std::endl;
                           std::cout << acc::ansi::foreground_yellow << "ID : " << acc::ansi::reset << std::endl;
                           declstmt->name.print_token();
                           std::cout << acc::ansi::foreground_yellow << "HISTORY : " << acc::ansi::reset << std::endl;
                           std::cout << acc::ansi::foreground_blue << "[ ";
                           for (const auto& v : declstmt->history) {
                               std::cout << acc::ansi::foreground_blue << " { " << acc::ansi::reset << std::endl;
                               print_expression(v);
                               std::cout << acc::ansi::foreground_blue << " }, " << acc::ansi::reset << std::endl;
                           }
                           std::cout << acc::ansi::foreground_blue << " ]" << acc::ansi::reset << std::endl;
                           if (declstmt->expr.has_value()) {
                               std::cout << acc::ansi::foreground_yellow << "CURRENT VALUE : " << acc::ansi::reset << std::endl;
                               this->print_expression(declstmt->expr.value());
                           }
                       },
                       [this](const acc::node::ExpressionStmt* xprstmt) {
                           std::cout << acc::ansi::foreground_green << "\n\n( EXPRESSION )\n\n"
                                     << acc::ansi::reset << std::endl;
                           this->print_expression(xprstmt->expr);
                       },
                       [this](const acc::node::WhileStmt* whilestmt) {
                           std::cout << acc::ansi::foreground_green << "\n\n( WHILE )\n\n"
                                     << acc::ansi::reset << std::endl;
                           std::cout << acc::ansi::foreground_yellow << "CONDITION : " << acc::ansi::reset << std::endl;
                           print_expression(whilestmt->condition);
                           std::cout << acc::ansi::foreground_yellow << "BODY : " << acc::ansi::reset << std::endl;
                           this->print_statement(whilestmt->body);
                       },
                       [this](const acc::node::BlockStmt* bstmt) {
                           std::cout << acc::ansi::foreground_green << "\n\n( BLOCK )\n\n"
                                     << acc::ansi::reset << std::endl;
                           std::cout << acc::ansi::foreground_yellow << "BODY : " << acc::ansi::reset << std::endl;
                           for (const auto& s : bstmt->stmts) {
                               this->print_statement(s);
                           }
                       },
                       [this](const acc::node::ForStmt* cxpr) {

                       }},
                   stmt);
        m_depth--;
    };

    printer(const std::vector<acc::StmtVariant>& st) : stmts(st) {};
    void print() {
        for (const auto& s : stmts) {
            print_statement(s);
        }
    };
};
}  // namespace acc