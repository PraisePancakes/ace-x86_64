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
        using namespace acc::exceptions;
        return std::visit(internal::visitor{
                              [](const BinaryExpr* expr) -> acc::node::info::type_info* {
                                  auto Tlhs = evaluate_type(expr->lhs);
                                  auto Trhs = evaluate_type(expr->rhs);
                                  if (are_equal(Tlhs, Trhs)) return Tlhs;
                                  throw exceptions::type_error({Tlhs, Trhs}, " conversion not allowed : ");
                              },
                              [](const UnaryExpr* expr) -> acc::node::info::type_info* {
                                  auto Texpr = evaluate_type(expr->expr);
                                  if (is_boolean(Texpr)) {
                                      return Texpr;
                                  }
                                  throw exceptions::type_error({Texpr}, " unsupported unary operation : ");
                              },
                              [](const LiteralExpr* expr) -> acc::node::info::type_info* {
                                  return expr->type;
                              },
                              [](const GroupingExpr* expr) -> acc::node::info::type_info* {
                                  return evaluate_type(expr->expr);
                              },
                              [](const CallExpr* expr) -> acc::node::info::type_info* {
                                  // we've fucking got equal arity between call args and procedure params here dont do shit with arity
                                  std::size_t param_index = 0;
                                  for (std::size_t i = 0; i < expr->args.size(); i++) {
                                      auto Targ = evaluate_type(expr->args[i]);
                                      auto Tparam = expr->procedure->params[param_index]->type;
                                      if (!are_equal(Targ, Tparam)) throw exceptions::type_error({Targ, Tparam}, " mismatched argument and paramater types : ");
                                  }
                                  return expr->procedure->type;
                              },
                              [](const VariableExpr* expr) -> acc::node::info::type_info* {
                                  return expr->type;
                              },
                              [](const AssignmentExpr* expr) -> acc::node::info::type_info* {
                                  auto Ta = evaluate_type(expr->expr);
                                  auto Tvar = expr->type;
                                  if (!are_equal(Ta, Tvar)) {
                                      throw exceptions::type_error({Ta, Tvar}, " mismatched argument and paramater types : ");
                                  };
                                  return Ta;
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

    static bool is_pointer(acc::node::info::type_info* t) {
        return std::holds_alternative<acc::node::info::Pointer>(t->type);
    }

    static bool is_boolean(acc::node::info::type_info* t) {
        return (is_pointer(t) || (std::holds_alternative<std::pair<acc::types::TYPES, acc::token>>(t->type) &&
                                  (acc::types::check_valid_implicit_conversion(std::get<std::pair<acc::types::TYPES, acc::token>>(t->type).first,
                                                                               acc::types::TYPES::BOOL))));
    };
};
}  // namespace acc::types