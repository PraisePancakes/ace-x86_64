#pragma once
#include <any>
#include <iostream>
#include <variant>

namespace acc {
namespace expr {
struct Literal {
    std::any value;
    std::string embedded;
    
};
struct BinaryExpr {};
struct UnaryExpr {};
}  // namespace expr

namespace stmt {
struct IfStmt {};
struct ThenStmt {};
struct WhileStmt {};
struct BlockStmt {};
}  // namespace stmt

using ExprVariant = std::variant<expr::BinaryExpr*,
                                 expr::UnaryExpr*>;

using StmtVariant = std::variant<stmt::IfStmt*,
                                 stmt::ThenStmt*,
                                 stmt::WhileStmt*,
                                 stmt::BlockStmt>;
}  // namespace acc