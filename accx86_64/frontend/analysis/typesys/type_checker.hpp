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
const std::array<std::array<bool, 6>, 6> implicit_cast_table = {
    //      bool , char, short, int,long, long long
    {/*bool*/ {1, 0, 0, 1, 0, 0},
     /*char*/ {1, 1, 1, 1, 1, 0},
     /*short*/ {1, 1, 1, 1, 1, 0},
     /*int*/ {1, 1, 1, 1, 1, 0},
     /*long*/ {1, 1, 1, 1, 1, 0},
     /*long long*/ {1, 1, 1, 1, 1, 0}},
};

struct type_checker final {
    static bool check_valid_implicit_conversion(TYPES t1, TYPES t2) noexcept {
        return implicit_cast_table[std::to_underlying(t1)][std::to_underlying(t2)];
    };

    // this will be one of the many conversion strategies, we must add floating point conversions as well as polymorphic conversions etc...

    static TYPES evaluate_type(const ExprVariant expr) {
        using acc::utils::type_inspector;

        return std::visit(internal::visitor{
                              [](const acc::node::BinaryExpr* expr) -> TYPES {
                                  auto Tl = type_checker::evaluate_type(expr->lhs);
                                  auto Tr = type_checker::evaluate_type(expr->rhs);
                                  if (type_checker::check_valid_implicit_conversion(Tl, Tr)) {
                                      return Tl >= Tr ? Tl : Tr;
                                  }

                                  throw exceptions::type_error({Tl, Tr}, " conversion not allowed : ");
                              },
                              [](const acc::node::LiteralExpr* expr) -> TYPES {
                                  auto type = type_inspector::to_type(expr->embedded);

                                  if (type.has_value()) {
                                      return type.value();
                                  }
                                  throw exceptions::type_error({type.value()}, " conversion not allowed : ");
                              },
                              [](const acc::node::GroupingExpr* expr) -> TYPES {
                                  return type_checker::evaluate_type(expr->expr);
                              },
                              [](const acc::node::CallExpr* expr) -> TYPES {
                                  // ensure call site args are the same type as params of the reference function.
                                  std::size_t ref_index = 0;
                                  for (std::size_t i = 0; i < expr->args.size(); i++) {
                                      auto* param = expr->procedure->params[ref_index++];
                                      auto Targ = evaluate_type(expr->args[i]);
                                      auto Tparam = type_inspector::to_type(param->type);
                                      if (!type_checker::check_valid_implicit_conversion(Targ, Tparam.value())) {
                                          throw exceptions::type_error({Targ, Tparam.value()}, " mismatched argument and paramater types : ");
                                      };
                                  };

                                  // return type of call
                                  return type_inspector::to_type(expr->procedure->type).value();
                              },
                              [](const acc::node::UnaryExpr* expr) -> TYPES {
                                  switch (expr->op.type) {
                                      case TK_BANG:
                                          if (type_checker::evaluate_type(expr->expr) == TYPES::BOOL) {
                                              return TYPES::BOOL;
                                          }
                                          break;
                                      case TK_DASH:
                                          if (type_checker::evaluate_type(expr->expr) == TYPES::INT) {
                                              return TYPES::INT;
                                          }
                                          break;
                                      case TK_PLUS:
                                          if (type_checker::evaluate_type(expr->expr) == TYPES::INT) {
                                              return TYPES::INT;
                                          }
                                          break;
                                      default:
                                          break;
                                  }

                                  throw exceptions::type_error({type_inspector::to_type(expr->op).value()}, " unsupported unary type operation : ");
                              },
                              [](const acc::node::VariableExpr* expr) -> TYPES {
                                  return type_inspector::to_type(expr->type).value();
                              },
                              [](const acc::node::AssignmentExpr* expr) -> TYPES {
                                  auto Torigin = type_inspector::to_type(expr->type);
                                  auto Texpr = type_checker::evaluate_type(expr->expr);
                                  if (!type_checker::check_valid_implicit_conversion(Torigin.value(), Texpr)) {
                                      throw exceptions::type_error({Torigin.value(), Texpr}, " mismatched types with assignment : ");
                                  }
                                  return Texpr;
                              },
                              [](std::monostate) -> TYPES { std::unreachable(); },
                          },
                          expr);
    };
};
}  // namespace acc::types