#pragma once
#include <any>
#include <iostream>
#include <memory>
#include <optional>
#include <unordered_set>
#include <variant>

#include "accenv.hpp"
#include "acctoken.hpp"

namespace acc {
// forwards
namespace node {
struct LiteralExpr;
struct BinaryExpr;
struct UnaryExpr;
struct GroupingExpr;
struct ComparisonExpr;
struct CallExpr;
struct VariableExpr;

struct IfStmt;
struct WhileStmt;
struct BlockStmt;
struct ForStmt;
struct DeclarationStmt;
struct ExpressionStmt;
struct FuncStmt;

}  // namespace node

// TO DO parser should return ownership of these nodes. It is not the parsers job to free this memory!!!!
// template <typename T>
// using uptr = std::unique_ptr<T>;
// using ExprOwningVariant = std::variant<uptr<node::BinaryExpr>,
//                                        uptr<node::UnaryExpr>,
//                                        uptr<node::LiteralExpr>,
//                                        uptr<node::GroupingExpr>>;

// TO DO implement assignment expression
using ExprVariant = std::variant<std::monostate,
                                 node::BinaryExpr*,
                                 node::UnaryExpr*,
                                 node::LiteralExpr*,
                                 node::GroupingExpr*,
                                 node::CallExpr*,
                                 node::VariableExpr*>;

using StmtVariant = std::variant<std::monostate,
                                 node::IfStmt*,
                                 node::WhileStmt*,
                                 node::BlockStmt*,
                                 node::ForStmt*,
                                 node::DeclarationStmt*,
                                 node::ExpressionStmt*,
                                 node::FuncStmt*>;

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

struct CallExpr {
    std::vector<ExprVariant> args;
};

struct VariableExpr {
    acc::token name;
    ExprVariant deduced;
};

struct DeclarationStmt {
    acc::token type;
    acc::token name;
    // const by default
    std::byte cv_qual_flags{0};  // 0000 0000 -lsb = const : lsb << 1 = volatile

    // DEBUG
    std::vector<ExprVariant> history;
    //

    static std::byte mask_const() noexcept {
        return std::byte{1 << 0};
    };

    static std::byte mask_volatile() noexcept {
        return std::byte{1 << 1};
    }

    static bool has_const(std::byte signature) noexcept {
        return ((signature & acc::node::DeclarationStmt::mask_const()) == acc::node::DeclarationStmt::mask_const());
    }
    static bool has_volatile(std::byte signature) noexcept {
        return ((signature & acc::node::DeclarationStmt::mask_volatile()) == acc::node::DeclarationStmt::mask_volatile());
    };

    static void remove_const(std::byte& signature) noexcept {
        (signature = ((signature) & (~std::byte{1})));
    };

    static void set_volatile(std::byte& signature) noexcept {
        (signature |= acc::node::DeclarationStmt::mask_volatile());
    };

    std::optional<ExprVariant> expr;  // resulant literal must match type
};

struct IfStmt {
    ExprVariant condition;
    StmtVariant then;
    std::optional<StmtVariant> else_;
};

struct ExpressionStmt {
    ExprVariant expr;
};

struct WhileStmt {
    ExprVariant condition;
    StmtVariant body;
};
struct BlockStmt {
    acc::environment<std::string, acc::StmtVariant>* env;
};
struct ForStmt {
    StmtVariant init;
    StmtVariant condition;
    ExprVariant expr;
    StmtVariant body;
};

/*
    int f() {};
    int f1 = () => return 2;
    int f2 = (int z, int y) => return z + y;
    .
    .
    .
    int x = f();
    int y = f1();
    int z = f2(1, 2);
*/

struct FuncStmt {
    acc::token type;
    acc::token name;
    std::vector<DeclarationStmt*> params;
    StmtVariant body;  // arrow or block
};

}  // namespace node

}  // namespace acc