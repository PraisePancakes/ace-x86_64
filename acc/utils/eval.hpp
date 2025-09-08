#pragma once
#include <iostream>
#include <variant>

#include "../frontend/accast.hpp"
#include "../traits/arithmeticable.hpp"
#include "visitor.hpp"

namespace acc::interp {
class expr_eval {
    [[nodiscard]] auto to_literal(acc::token::value_type var) const noexcept {
        return std::visit([](const auto v) -> acc::token::value_type { return v; }, var);
    };

    [[nodiscard]] acc::token::value_type evaluate(acc::ExprVariant exp) const {
        return std::visit(internal::visitor{
                              [&](acc::node::BinaryExpr* bxpr) -> acc::token::value_type {
                                  acc::token::value_type l = evaluate(bxpr->lhs);
                                  acc::token::value_type r = evaluate(bxpr->rhs);
                                  return std::visit(internal::visitor{
                                                        [&bxpr]<acc::traits::arithmeticable T>(T a, T b) -> acc::token::value_type {
                                                            switch (bxpr->op.type) {
                                                                case ACC_ALL_TOKEN_ENUM::TK_PLUS:
                                                                    return a + b;
                                                                case ACC_ALL_TOKEN_ENUM::TK_DASH:
                                                                    return a - b;
                                                                case ACC_ALL_TOKEN_ENUM::TK_STAR:
                                                                    return a * b;
                                                                case ACC_ALL_TOKEN_ENUM::TK_SLASH:
                                                                    return a / b;
                                                                default:
                                                                    throw std::runtime_error("undefined binary operator");
                                                            };
                                                        },
                                                        [](auto, auto) -> acc::token::value_type {
                                                            throw std::runtime_error("mismatched type binary evaluation");
                                                        }},
                                                    to_literal(l), to_literal(r));
                              },
                              [&](acc::node::LiteralExpr* lxpr) -> acc::token::value_type {
                                  return to_literal(lxpr->value);
                              },
                              [&](acc::node::UnaryExpr* uxpr) -> acc::token::value_type {
                                  acc::token::value_type expr = evaluate(uxpr->expr);
                                  return to_literal(expr);
                              },
                              [&](acc::node::GroupingExpr* gxpr) -> acc::token::value_type {
                                  acc::token::value_type expr = evaluate(gxpr->expr);
                                  return to_literal(expr);
                              },
                          },
                          exp);
    };

   public:
    expr_eval() {};

    template <typename T>
    T as(acc::ExprVariant expr) {
        return std::get<T>(evaluate(expr));
    }
    ~expr_eval() {};
};
};  // namespace acc::interp