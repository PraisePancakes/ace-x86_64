#pragma once
#include <iostream>
#include <vector>

#include "../utils/ansi.hpp"
#include "../utils/eval.hpp"
#include "../utils/inker.hpp"
#include "accast.hpp"

// TO DO make this shit not shit :(
namespace acc {
class printer {
    std::size_t m_depth = 0;
    std::vector<acc::StmtVariant> stmts;

   public:
    void print_expression(const acc::ExprVariant expr, std::ostream& os) {
        // non owning view of nodes
        acc::utils::inker inker(os);
        m_depth++;
        inker.print_depth(m_depth, '-');
        std::visit(internal::visitor{
                       [this, &inker](const acc::node::BinaryExpr* bxpr) {
                           inker.print_green(" [ BINARY ] ");
                           inker.print_depth(m_depth, ' ');
                           std::cout << "|";
                           print_expression(bxpr->lhs, inker.os);
                           inker.print_depth(m_depth, ' ');
                           inker.print_red(bxpr->op.word);
                           inker.print_depth(m_depth, ' ');
                           std::cout << "|";
                           print_expression(bxpr->rhs, inker.os);
                       },
                       [this, &inker](const acc::node::LiteralExpr* lxpr) {
                           inker.print_green(" [ LITERAL ] ");
                           inker.print_depth(m_depth, ' ');

                           inker.print_yellow(lxpr->embedded.word);
                       },
                       [this, &inker](const acc::node::UnaryExpr* uexpr) {
                           inker.print_green(" [ UNARY ] ");

                           inker.print_red(uexpr->op.word);
                           print_expression(uexpr->expr, inker.os);
                       },
                       [this, &inker](const acc::node::GroupingExpr* gexpr) {
                           inker.print_green(" [ GROUP ] ");
                           inker.print_depth(m_depth, ' ');
                           inker.print_blue("|");
                           print_expression(gexpr->expr, inker.os);
                       },
                       [this, &inker](const acc::node::VariableExpr* vexpr) {
                           inker.print_green(" [ VARIABLE ] ");
                           inker.print_depth(m_depth, ' ');

                           inker.print_yellow(vexpr->name.word);
                       },
                       [this, &inker]([[maybe_unused]] const acc::node::CallExpr* cexpr) {

                       },
                   },
                   expr);
        m_depth--;
    };

    void print_statement(const acc::StmtVariant stmt, std::ostream& os) {
        acc::utils::inker inker(os);
        m_depth++;
        inker.print_depth(m_depth, '-');
        std::visit(internal::visitor{
                       [this, &inker](const acc::node::IfStmt* ifstmt) {
                           inker.print_green("( IF )");
                           std::cout << acc::ansi::foreground_yellow << "CONDITION : " << acc::ansi::reset << std::endl;
                           print_expression(ifstmt->condition, inker.os);
                           this->print_statement(ifstmt->then, inker.os);
                           if (ifstmt->else_.has_value()) {
                               inker.print_green("( ELSE )");
                               this->print_statement(ifstmt->else_.value(), inker.os);
                           }
                       },
                       [this, &inker](const acc::node::DeclarationStmt* declstmt) {
                           bool has_const = declstmt->has_const(declstmt->cv_qual_flags);
                           bool has_volatile = declstmt->has_volatile(declstmt->cv_qual_flags);

                           inker.print_green("( DECLARATION )");
                           inker.print_yellow("TYPE : ", declstmt->type.word);
                           inker.print_yellow("QUALIFIERS : [ ", (has_const ? "const" : " "), " ", (has_volatile ? "volatile" : " "), "]");
                           inker.print_yellow("ID : ", declstmt->name.word);
                           inker.print_yellow("HISTORY : [");
                           for (const auto& v : declstmt->history) {
                               inker.print_blue(" { ");
                               print_expression(v, inker.os);
                               inker.print_blue(" }, ");
                           }
                           inker.print_yellow("]");
                           if (declstmt->expr.has_value()) {
                               inker.print_yellow("CURRENT VALUE : {");
                               this->print_expression(declstmt->expr.value(), inker.os);
                               inker.print_yellow("}");
                           }
                       },
                       [this, &inker](const acc::node::ExpressionStmt* xprstmt) {
                           inker.print_green("( EXPRESSION )");
                           this->print_expression(xprstmt->expr, inker.os);
                       },
                       [this, &inker](const acc::node::WhileStmt* whilestmt) {
                           inker.print_green("\n\n( WHILE )\n\n");
                           inker.print_yellow("CONDITION : ");
                           print_statement(whilestmt->condition, inker.os);
                           inker.print_yellow("BODY : ");
                           this->print_statement(whilestmt->body, inker.os);
                       },
                       [this, &inker](const acc::node::BlockStmt* bstmt) {
                           inker.print_green("( BLOCK )");
                           inker.print_yellow("BODY :");
                           for (const auto& s : bstmt->stmts) {
                               this->print_statement(s, inker.os);
                           }
                       },
                       [this, &inker](const acc::node::ForStmt* cxpr) {
                           inker.print_green("( FOR )");
                           std::cout << acc::ansi::foreground_yellow << "INIT : " << acc::ansi::reset << std::endl;
                           print_statement(cxpr->init, inker.os);
                           std::cout << acc::ansi::foreground_yellow << "CONDITION : " << acc::ansi::reset << std::endl;
                           print_statement(cxpr->condition, inker.os);
                           std::cout << acc::ansi::foreground_yellow << "EXPR : " << acc::ansi::reset << std::endl;
                           print_expression(cxpr->expr, inker.os);
                           inker.print_yellow("BODY :");
                           print_statement(cxpr->body, inker.os);
                       },
                       [this, &inker](const acc::node::FuncStmt* fstmt) {
                           inker.print_green("( FUNC )");
                           std::cout << acc::ansi::foreground_yellow << "TYPE : " << acc::ansi::reset << std::endl;
                           fstmt->type.write_token(std::cout);
                           std::cout << acc::ansi::foreground_yellow << "NAME : " << acc::ansi::reset << std::endl;
                           fstmt->name.write_token(std::cout);
                           std::cout << acc::ansi::foreground_yellow << "PARAMS : " << acc::ansi::reset << std::endl;
                           for (auto& p : fstmt->params) {
                               print_statement(p, inker.os);
                           };
                           inker.print_yellow("BODY :");
                           print_statement(fstmt->body, inker.os);
                       }},
                   stmt);
        m_depth--;
    };

    printer(const std::vector<acc::StmtVariant>& st) : stmts(st) {};
    void print(std::ostream& os) {
        for (const auto& s : stmts) {
            print_statement(s, os);
        }
    };
};
}  // namespace acc