#pragma once
#include <iostream>
#include <vector>

#include "../utils/ansi.hpp"
#include "accast.hpp"

namespace acc {
class printer {
    std::size_t m_depth = 0;
    std::vector<acc::ExprVariant> exprs;

   public:
    void print_expression(acc::ExprVariant expr) {
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
                                     }},
                   expr);
        m_depth--;
    };

    void print_statement() {
    };

    printer(const std::vector<acc::ExprVariant>& xpr) : exprs(xpr) {};
    void print() {
        for (const auto& e : exprs) {
            print_expression(e);
        }
    };
};
}  // namespace acc