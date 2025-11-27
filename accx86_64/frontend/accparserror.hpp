#pragma once
#include <iostream>

#include "../utils/acclog.hpp"
#include "acctoken.hpp"

namespace acc {
struct parser_error : std::exception {
    acc::token token;
    std::string what;

    parser_error(acc::token t, const std::string& w) : token(t), what(w) {};
    virtual ~parser_error() = default;
};
}  // namespace acc