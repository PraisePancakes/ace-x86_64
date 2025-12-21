#pragma once
#include <array>
#include <iostream>
#include <optional>
#include <variant>

#include "../../accast.hpp"
#include "../../utils/type_inspector.hpp"
#include "type_error.hpp"
#include "types.hpp"

namespace acc::types {

struct type_checker final {
    // this will be one of the many conversion strategies, we must add floating point conversions as well as polymorphic conversions etc...

    static acc::node::info::type_info* evaluate_type(const ExprVariant expr) {
        using namespace acc::node;
        return std::visit(internal::visitor{
                              [](const BinaryExpr* expr) -> acc::node::info::type_info* {
                                  auto lhs = evaluate_type(expr->lhs);
                                  auto rhs = evaluate_type(expr->rhs);
                                  if (are_equal(lhs, rhs)) return lhs;

                                  return nullptr;
                              },
                              [](const UnaryExpr* expr) -> acc::node::info::type_info* {
                                  return nullptr;
                              },
                              [](const LiteralExpr* expr) -> acc::node::info::type_info* {
                                  return nullptr;
                              },
                              [](const GroupingExpr* expr) -> acc::node::info::type_info* {
                                  return nullptr;
                              },
                              [](const CallExpr* expr) -> acc::node::info::type_info* {
                                  return nullptr;
                              },
                              [](const VariableExpr* expr) -> acc::node::info::type_info* {
                                  return nullptr;
                              },
                              [](const AssignmentExpr* expr) -> acc::node::info::type_info* {
                                  return nullptr;
                              },
                              [](const std::monostate) -> acc::node::info::type_info* {
                                  return nullptr;
                              }},
                          expr);
        return nullptr;
    };

    static bool is_type(const ExprVariant expr, acc::node::info::type_info* t) {
        return evaluate_type(expr) == t;
    }
    static bool are_equal(const ExprVariant e1, const ExprVariant e2) {
        return evaluate_type(e1) == evaluate_type(e2);
    }

    static bool are_equal(acc::node::info::type_info* t1, acc::node::info::type_info* t2) {
        return (t1 && t2 && (t1->type == t2->type));
    };
};
}  // namespace acc::types