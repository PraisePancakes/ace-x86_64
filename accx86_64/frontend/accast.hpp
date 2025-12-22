#pragma once
#include <any>
#include <iostream>
#include <memory>
#include <optional>
#include <unordered_set>
#include <variant>

#include "accenv.hpp"
#include "acctoken.hpp"
#include "analysis/typesys/type_conversions.hpp"

namespace acc {
// forwards
namespace node {
struct LiteralExpr;
struct BinaryExpr;
struct UnaryExpr;
struct GroupingExpr;
struct CallExpr;
struct VariableExpr;
struct AssignmentExpr;

struct DeclarationStmt;
struct IfStmt;
struct WhileStmt;
struct BlockStmt;
struct ForStmt;
struct ExpressionStmt;
struct FuncStmt;
struct ReturnStmt;
struct TypeStmt;

}  // namespace node

// TO DO parser should transfer ownership of these nodes. It is not the parsers job to free this memory..
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
                                 node::VariableExpr*,
                                 node::AssignmentExpr*>;

using StmtVariant = std::variant<std::monostate,
                                 node::IfStmt*,
                                 node::WhileStmt*,
                                 node::BlockStmt*,
                                 node::ForStmt*,
                                 node::DeclarationStmt*,
                                 node::ExpressionStmt*,
                                 node::FuncStmt*,
                                 node::ReturnStmt*,
                                 node::TypeStmt*>;

namespace node {

namespace info {
struct type_info;
struct Pointer;
struct Array;

struct Pointer {
    type_info* pointee{nullptr};
};
struct Array {
    int len;
    type_info* element{nullptr};
};
struct type_info {
    std::variant<std::pair<acc::types::TYPES, acc::token>, info::Pointer, info::Array> type;
};

inline bool operator==(const Pointer& lhs, const Pointer& rhs) {
    return (lhs.pointee && rhs.pointee) && (lhs.pointee->type == rhs.pointee->type);
};

inline bool operator==(const Array& lhs, const Array& rhs) {
    return (lhs.len == rhs.len) && (lhs.element && rhs.element) && (lhs.element->type == rhs.element->type);
};

inline bool operator==(const std::pair<acc::types::TYPES, acc::token>& lhs, const std::pair<acc::types::TYPES, acc::token>& rhs) {
    if (lhs.first == acc::types::TYPES::CLASS) {
        return lhs.second.word == rhs.second.word;
    }
    return lhs.first == rhs.first || (types::check_valid_implicit_conversion(lhs.first, rhs.first));
};

}  // namespace info

constexpr static bool PRIVATE = true;
constexpr static bool PUBLIC = false;
// struct IfStmt;
// struct ThenStmt;
// struct WhileStmt;
// struct BlockStmt;

struct LiteralExpr {
    info::type_info* type{nullptr};
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
    FuncStmt* procedure;
};

struct VariableExpr {
    info::type_info* type{nullptr};
    acc::token name;
    std::optional<ExprVariant> evaluated{std::nullopt};
};

struct AssignmentExpr {
    info::type_info* type{nullptr};
    acc::token name;
    ExprVariant expr;
};

struct DeclarationStmt {
    info::type_info* type{nullptr};
    std::optional<bool> access_specifier{0};  // true private, false public
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
    acc::node::BlockStmt* then;
    std::optional<acc::node::BlockStmt*> else_;
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
    ReturnStmt* ret{nullptr};
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
    info::type_info* type{nullptr};
    acc::token name;
    std::vector<DeclarationStmt*> params;
    std::optional<bool> access_specifier;  // true private, false public
    BlockStmt* body;                       // block
};

struct ReturnStmt {
    acc::token keyword;
    acc::ExprVariant expr;
};

struct TypeStmt {
    acc::token type_name;
    acc::environment<std::string, acc::StmtVariant>* members;
};

}  // namespace node

}  // namespace acc