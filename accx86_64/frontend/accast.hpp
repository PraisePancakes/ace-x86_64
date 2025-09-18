#pragma once
#include <any>
#include <iostream>
#include <memory>
#include <optional>
#include <unordered_set>
#include <variant>

#include "acctoken.hpp"

namespace acc {
// forwards
namespace node {
struct LiteralExpr;
struct BinaryExpr;
struct UnaryExpr;
struct GroupingExpr;
struct ComparisonExpr;

struct IfStmt;
struct WhileStmt;
struct BlockStmt;
struct ForStmt;
struct DeclarationStmt;
struct ExpressionStmt;

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
                                 node::GroupingExpr*,
                                 node::ComparisonExpr*>;

using StmtVariant = std::variant<node::IfStmt*,
                                 node::WhileStmt*,
                                 node::BlockStmt*,
                                 node::ForStmt*,
                                 node::DeclarationStmt*,
                                 node::ExpressionStmt*>;

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

struct ComparisonExpr {
    ExprVariant lhs;
    ExprVariant rhs;
    acc::token op;
};

struct DeclarationStmt {
    acc::token type;
    acc::token name;
    std::byte cv_qual_flags;  // 0000 0000 -lsb = const : lsb << 1 = volatile
    static std::byte mask_const() noexcept {
        return std::byte{1 << 0};
    };

    static std::byte mask_volatile() noexcept {
        return std::byte{1 << 1};
    }
    std::optional<ExprVariant> expr;  // resulant literal must match type
};

struct IfStmt {
    ExprVariant condition;
    StmtVariant then;
    StmtVariant else_;
};

struct ExpressionStmt {
    ExprVariant expr;
};

struct WhileStmt {
    ExprVariant condition;
    StmtVariant body;
};
struct BlockStmt {
    std::vector<StmtVariant> stmts;
};
struct ForStmt {
};

}  // namespace node

}  // namespace acc