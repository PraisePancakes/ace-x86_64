#pragma once
#include <iostream>

#include "../../accast.hpp"
#include "integral_types.hpp"
namespace acc::exceptions {

struct type_error : std::exception {
    std::string what;
    std::vector<acc::types::INTEGRAL_TYPES> inconvertible_types;

    type_error(const std::vector<acc::types::INTEGRAL_TYPES>& ts, const std::string& w) : what(w), inconvertible_types{ts} {}
    virtual ~type_error() = default;
};
};  // namespace acc::exceptions