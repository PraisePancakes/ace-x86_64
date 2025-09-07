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

struct IfStmt;
struct ThenStmt;
struct WhileStmt;
struct BlockStmt;
struct ForStmt;

}  // namespace node


// TO DO parser should return ownership of these nodes. It is not the parsers job to free this memory!!!!
// template <typename T>
// using uptr = std::unique_ptr<T>;
// using ExprOwningVariant = std::variant<uptr<node::BinaryExpr>,
//                                        uptr<node::UnaryExpr>,
//                                        uptr<node::LiteralExpr>,
//                                        uptr<node::GroupingExpr>>;
using ExprVariant = std::variant<node::BinaryExpr*,
                                 node::UnaryExpr*,
                                 node::LiteralExpr*,
                                 node::GroupingExpr*>;

using StmtVariant = std::variant<node::IfStmt*,
                                       node::ThenStmt*,
                                       node::WhileStmt*,
                                       node::BlockStmt*,
                                       node::ForStmt*>;

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

struct IfStmt {};
struct ThenStmt {};
struct WhileStmt {};
struct BlockStmt {};
struct ForStmt {};

}  // namespace node


}  // namespace acc