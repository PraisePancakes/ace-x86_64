#pragma once
#include <iostream>
#include <vector>

#include "../utils/ansi.hpp"
#include "../utils/eval.hpp"
#include "accast.hpp"

// TO DO make this shit not shit :(
namespace acc {
class printer {
    std::size_t m_depth = 0;
    std::vector<acc::StmtVariant> stmts;

    template <typename... Args>
    void print_green(Args&&... args) const noexcept {
        (((std::cout << acc::ansi::foreground_green << args << acc::ansi::reset), ...) << std::endl);
    }
    template <typename... Args>
    void print_yellow(Args&&... args) const noexcept {
        (((std::cout << acc::ansi::foreground_yellow << args << acc::ansi::reset), ...) << std::endl);
    }
    template <typename... Args>
    void print_red(Args&&... args) const noexcept {
        (((std::cout << acc::ansi::foreground_light_red << args << acc::ansi::reset), ...) << std::endl);
    }

    template <typename... Args>
    void print_blue(Args&&... args) const noexcept {
        (((std::cout << acc::ansi::foreground_blue << args << acc::ansi::reset), ...) << std::endl);
    }

    void print_depth(std::size_t N, const char c) const noexcept {
        while (N--) {
            std::cout << c;
        }
    }

    /*
    [EXPRESSION]
    |
    ----[LITERAL]
    +




    */
   public:
    void print_expression(const acc::ExprVariant expr) {
        // non owning view of nodes
        m_depth++;
        print_depth(m_depth, '-');
        std::visit(internal::visitor{
                       [this](const acc::node::BinaryExpr* bxpr) {
                           print_green(" [ BINARY ] ");
                           print_depth(m_depth, ' ');
                           std::cout << "|";
                           print_expression(bxpr->lhs);
                           print_depth(m_depth, ' ');
                           print_red(bxpr->op.word);
                           print_depth(m_depth, ' ');
                           std::cout << "|";
                           print_expression(bxpr->rhs);
                       },
                       [this](const acc::node::LiteralExpr* lxpr) {
                           print_green(" [ LITERAL ] ");
                           print_depth(m_depth, ' ');

                           print_yellow(lxpr->embedded.word);
                       },
                       [this](const acc::node::UnaryExpr* uexpr) {
                           print_green(" [ UNARY ] ");

                           print_red(uexpr->op.word);
                           print_expression(uexpr->expr);
                       },
                       [this](const acc::node::GroupingExpr* gexpr) {
                           print_green(" [ GROUP ] ");
                           print_depth(m_depth, ' ');
                           std::cout << "|";
                           print_expression(gexpr->expr);
                       },
                       [this](const acc::node::VariableExpr* vexpr) {
                           print_green(" [ VARIABLE ] ");
                           print_depth(m_depth, ' ');

                           print_yellow(vexpr->name.word);
                       },
                       [this]([[maybe_unused]] const acc::node::CallExpr* cexpr) {

                       },
                   },
                   expr);
        m_depth--;
    };

    void print_statement(const acc::StmtVariant stmt) {
        m_depth++;
        print_depth(m_depth, '-');
        std::visit(internal::visitor{
                       [this](const acc::node::IfStmt* ifstmt) {
                           print_green("( IF )");
                           std::cout << acc::ansi::foreground_yellow << "CONDITION : " << acc::ansi::reset << std::endl;
                           print_expression(ifstmt->condition);
                           this->print_statement(ifstmt->then);
                           if (ifstmt->else_.has_value()) {
                               print_green("( ELSE )");
                               this->print_statement(ifstmt->else_.value());
                           }
                       },
                       [this](const acc::node::DeclarationStmt* declstmt) {
                           bool has_const = declstmt->has_const(declstmt->cv_qual_flags);
                           bool has_volatile = declstmt->has_volatile(declstmt->cv_qual_flags);

                           print_green("( DECLARATION )");
                           print_yellow("TYPE : ", declstmt->type.word);
                           print_yellow("QUALIFIERS : [ ", (has_const ? "const" : " "), " ", (has_volatile ? "volatile" : " "), "]");
                           print_yellow("ID : ", declstmt->name.word);
                           print_yellow("HISTORY : [");
                           for (const auto& v : declstmt->history) {
                               print_blue(" { ");
                               print_expression(v);
                               print_blue(" }, ");
                           }
                           print_yellow("]");
                           if (declstmt->expr.has_value()) {
                               print_yellow("CURRENT VALUE : {");
                               this->print_expression(declstmt->expr.value());
                               print_yellow("}");
                           }
                       },
                       [this](const acc::node::ExpressionStmt* xprstmt) {
                           print_green("( EXPRESSION )");
                           this->print_expression(xprstmt->expr);
                       },
                       [this](const acc::node::WhileStmt* whilestmt) {
                           std::cout << acc::ansi::foreground_green << "\n\n( WHILE )\n\n"
                                     << acc::ansi::reset << std::endl;
                           std::cout << acc::ansi::foreground_yellow << "CONDITION : " << acc::ansi::reset << std::endl;
                           print_statement(whilestmt->condition);
                           std::cout << acc::ansi::foreground_yellow << "BODY : " << acc::ansi::reset << std::endl;
                           this->print_statement(whilestmt->body);
                       },
                       [this](const acc::node::BlockStmt* bstmt) {
                           print_green("( BLOCK )");
                           print_yellow("BODY :");
                           for (const auto& s : bstmt->stmts) {
                               this->print_statement(s);
                           }
                       },
                       [this](const acc::node::ForStmt* cxpr) {
                           print_green("( FOR )");
                           std::cout << acc::ansi::foreground_yellow << "INIT : " << acc::ansi::reset << std::endl;
                           print_statement(cxpr->init);
                           std::cout << acc::ansi::foreground_yellow << "CONDITION : " << acc::ansi::reset << std::endl;
                           print_statement(cxpr->condition);
                           std::cout << acc::ansi::foreground_yellow << "EXPR : " << acc::ansi::reset << std::endl;
                           print_expression(cxpr->expr);
                           print_yellow("BODY :");
                           print_statement(cxpr->body);
                       },
                       [this](const acc::node::FuncStmt* fstmt) {
                           print_green("( FUNC )");
                           std::cout << acc::ansi::foreground_yellow << "TYPE : " << acc::ansi::reset << std::endl;
                           fstmt->type.write_token(std::cout);
                           std::cout << acc::ansi::foreground_yellow << "NAME : " << acc::ansi::reset << std::endl;
                           fstmt->name.write_token(std::cout);
                           std::cout << acc::ansi::foreground_yellow << "PARAMS : " << acc::ansi::reset << std::endl;
                           for (auto& p : fstmt->params) {
                               print_statement(p);
                           };
                           print_yellow("BODY :");
                           print_statement(fstmt->body);
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