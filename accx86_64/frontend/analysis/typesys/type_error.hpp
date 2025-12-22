#pragma once
#include <iostream>

#include "../../accast.hpp"
#include "types.hpp"
namespace acc::exceptions {

struct type_error : std::exception {
    std::string what;
    std::vector<acc::node::info::type_info*> inconvertible_types;

    type_error(const std::vector<acc::node::info::type_info*>& ts, const std::string& w) : what(w), inconvertible_types{ts} {}
    virtual ~type_error() = default;
};
};  // namespace acc::exceptions