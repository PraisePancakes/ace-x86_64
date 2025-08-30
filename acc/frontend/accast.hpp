#pragma once
#include <any>
#include <iostream>
#include <memory>
#include <variant>

#include "acctoken.hpp"

namespace acc {
namespace expr {
struct Literal {
    std::any value;
    acc::token embedded;
};
struct BinaryExpr {
    Literal lhs;
    Literal rhs;
    acc::token op;
};
struct UnaryExpr {
    Literal expr;
    acc::token uop;
};

}  // namespace expr

namespace stmt {
struct IfStmt {};
struct ThenStmt {};
struct WhileStmt {};
struct BlockStmt {};
}  // namespace stmt

template <typename T>
using uptr = std::unique_ptr<T>;
using ExprOwningVariant = std::variant<uptr<expr::BinaryExpr>,
                                       uptr<expr::UnaryExpr>,
                                       uptr<expr::Literal>>;

using StmtOwningVariant = std::variant<uptr<stmt::IfStmt>,
                                       uptr<stmt::ThenStmt>,
                                       uptr<stmt::WhileStmt>,
                                       uptr<stmt::BlockStmt>>;
}  // namespace acc