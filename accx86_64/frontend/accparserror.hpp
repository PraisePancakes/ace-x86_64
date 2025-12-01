#pragma once
#include <iostream>

#include "acctoken.hpp"

namespace acc::exceptions {
struct parser_error : std::exception {
    acc::token token;
    std::string what;

    parser_error(acc::token t, const std::string& w) : token(t), what(w) {};
    virtual ~parser_error() = default;
};
}  // namespace acc