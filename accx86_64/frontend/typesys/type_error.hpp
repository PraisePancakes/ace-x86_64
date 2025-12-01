#pragma once
#include <iostream>

#include "../accast.hpp"
namespace acc::exceptions {
struct type_error : std::exception {
    acc::ExprVariant expression;
    std::string what;

    type_error(acc::ExprVariant expr, const std::string& w) : expression(expr), what(w) {};
    virtual ~type_error() = default;
};
};  // namespace acc::exceptions