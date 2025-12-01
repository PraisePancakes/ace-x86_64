#pragma once
#include <array>
#include <iostream>
#include <optional>
#include <variant>

#include "../accast.hpp"
#include "type_error.hpp"
namespace acc::types {  //                                                  int , char, bool, short, long, long long
const std::array<std::array<bool, 6>, 6> integral_cast_bit_table = {
    {/*int*/ {1, 1, 1, 1, 1},
     /*char*/ {1, 1, 1, 1, 1},
     /*bool*/ {1, 0, 1, 1, 0},
     /*short*/ {1, 1, 1, 1, 1},
     /*long*/ {1, 1, 1, 1, 1},
     /*long long*/ {1, 1, 1, 1, 1}},
};

enum class INTEGRAL_TYPES : std::size_t {
    BOOL = 1,
    CHAR = 2,
    SHORT = 3,
    INT = 4,
    LONG = 5,
    LONG_LONG = 6,
};

class type_checker final {
   public:
    // this will be one of the many conversion strategies, we must add floating point conversions as well as polymorphic conversions etc...
    static bool check_valid_integral_conversion(INTEGRAL_TYPES t1, INTEGRAL_TYPES t2) noexcept {
        return integral_cast_bit_table[std::to_underlying(t1)][std::to_underlying(t2)];
    };

    static std::optional<INTEGRAL_TYPES> token_to_integral_type(acc::GLOBAL_TOKENS type) noexcept {
        switch (type) {
            case TK_LITERAL_BOOL:
                return INTEGRAL_TYPES::BOOL;
            case TK_LITERAL_CHAR:
                return INTEGRAL_TYPES::CHAR;
            case TK_LITERAL_SHORT:
                return INTEGRAL_TYPES::SHORT;
            case TK_LITERAL_INT:
                return INTEGRAL_TYPES::INT;
            case TK_LITERAL_LONG:
                return INTEGRAL_TYPES::LONG;
            case TK_LITERAL_LONG_LONG:
                return INTEGRAL_TYPES::LONG_LONG;
            default:
                return std::nullopt;
        }
    };

    static INTEGRAL_TYPES evaluate_type(ExprVariant expr) noexcept {
        return std::visit(internal::visitor{
                              [](acc::node::BinaryExpr* expr) -> INTEGRAL_TYPES {
                                  auto Tl = type_checker::evaluate_type(expr->lhs);
                                  auto Tr = type_checker::evaluate_type(expr->rhs);
                                  if (type_checker::check_valid_integral_conversion(Tl, Tr)) {
                                      return Tl >= Tr ? Tl : Tr;
                                  }

                                  throw exceptions::type_error(expr, " conversion not allowed : ");
                              },
                              [](acc::node::LiteralExpr* expr) -> INTEGRAL_TYPES {
                                  auto type = token_to_integral_type(expr->embedded.type);
                                  if (type.has_value()) {
                                      return type.value();
                                  }
                                  throw exceptions::type_error(expr, " undefined type : ");
                              },
                              [](acc::node::GroupingExpr* expr) -> INTEGRAL_TYPES {
                                  return type_checker::evaluate_type(expr->expr);
                              },
                              [](acc::node::CallExpr* expr) -> INTEGRAL_TYPES {
                                  // ensure call site args are the same type as params of the reference function.
                                  std::size_t ref_index = 0;
                                  for (std::size_t i = 0; i < expr->args.size(); i++) {
                                      auto* param = expr->procedure->params[ref_index];
                                      auto Targ = evaluate_type(expr->args[i]);
                                      auto Tparam = type_checker::token_to_integral_type(param->type.type);
                                      if (!type_checker::check_valid_integral_conversion(Targ, Tparam.value())) {
                                          throw exceptions::type_error(expr, " mismatched argument and paramater types : ");
                                      };
                                      ref_index++;
                                  };

                                  // return type of call
                                  return type_checker::token_to_integral_type(expr->procedure->type.type).value();
                              },
                              [](acc::node::UnaryExpr* expr) -> INTEGRAL_TYPES {
                                  // TO DO IMPLEMENT THIS
                                  // FOR NOW RETURN BOOL FCK IT
                                  return INTEGRAL_TYPES::BOOL;
                              },
                              [](acc::node::VariableExpr* expr) -> INTEGRAL_TYPES {
                                  return type_checker::token_to_integral_type(expr->type.type).value();
                              },
                              [](std::monostate) -> INTEGRAL_TYPES { throw exceptions::type_error(std::monostate{}, " type referred to monostate "); },
                          },
                          expr);
    };
};
}  // namespace acc::types