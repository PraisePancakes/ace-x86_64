#pragma once
#include <array>
#include <iostream>

#include "../accast.hpp"
namespace acc::types {  //                                                  int, float, char, bool, short, long, double
const std::array<std::array<bool, 7>, 7> cast_bit_table = {{/*int*/ {1, 1, 1, 1, 1, 1, 1},
                                                            /*float*/ {1, 1, 1, 1, 1, 1, 1},
                                                            /*char*/ {1, 0, 1, 1, 1, 1, 0},
                                                            /*bool*/ {1, 0, 0, 1, 1, 0, 0},
                                                            /*short*/ {1, 1, 1, 1, 1, 1, 1},
                                                            /*long*/ {1, 1, 1, 1, 1, 1, 1},
                                                            /*double*/ {1, 1, 1, 1, 1, 1, 1}}};
enum class TYPES : std::size_t {
    INT,
    FLOAT,
    CHAR,
    BOOL,
    SHORT,
    LONG,
    DOUBLE
};

class type_checker final {
   public:
    static bool check_valid_conversion(TYPES t1, TYPES t2) noexcept {
        return cast_bit_table[std::to_underlying(t1)][std::to_underlying(t2)];
    };

    static std::optional<TYPES> evaluate_type(ExprVariant expr) noexcept {
        return std::visit(internal::visitor{
                              [](acc::node::BinaryExpr* expr) {
                                  auto Tl = evaluate_type(expr->lhs);
                                  auto Tr = evaluate_type(expr->rhs);
                                  if (Tl.has_value() && Tr.has_value()) {
                                      if (check_valid_conversion(Tl.value(), Tr.value())) {
                                          // check rank, lower rank will be converted type.
                                      }
                                  }
                                  return std::nullopt;
                              },
                              [](acc::node::LiteralExpr* expr) {
                                  switch (expr->embedded.type) {
                                      case TK_LITERAL_CHAR:
                                          return TYPES::CHAR;
                                      case TK_LITERAL_DOUBLE:
                                          return TYPES::DOUBLE;
                                      case TK_LITERAL_INT:
                                          return TYPES::INT;
                                  }
                              },
                              [](acc::node::GroupingExpr expr) {},
                              [](acc::node::CallExpr* expr) {},
                              [](acc::node::UnaryExpr* expr) {},
                              [](acc::node::VariableExpr* expr) {},
                              [](std::monostate) { return std::nullopt; },
                          },
                          expr);
    };
};
}  // namespace acc::types