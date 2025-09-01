#pragma once
#include <any>
#include <iostream>
#include <memory>
#include <variant>

#include "acctoken.hpp"

namespace acc {
// forwards
namespace node {
struct LiteralExpr;
struct BinaryExpr;
struct UnaryExpr;

struct GroupingExpr;
}  // namespace node
using ExprVariant = std::variant<node::BinaryExpr*,
                                 node::UnaryExpr*,
                                 node::LiteralExpr*,
                                 node::GroupingExpr*>;

namespace node {
// struct IfStmt;
// struct ThenStmt;
// struct WhileStmt;
// struct BlockStmt;

struct LiteralExpr {
    token::value_type value;
    acc::token embedded;
};
struct BinaryExpr {
    ExprVariant lhs;
    ExprVariant rhs;
    acc::token op;
};
struct UnaryExpr {
    acc::token op;
    ExprVariant expr;
};

struct GroupingExpr {
    ExprVariant expr;
};

// struct IfStmt {};
// struct ThenStmt {};
// struct WhileStmt {};
// struct BlockStmt {};

}  // namespace node

// using StmtOwningVariant = std::variant<node::IfStmt*,
//                                        node::ThenStmt*,
//                                        node::WhileStmt*,
//                                        node::BlockStmt*>;
}  // namespace acc