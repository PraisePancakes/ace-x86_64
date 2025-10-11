#pragma once
#include <iostream>
#include <unordered_map>

#include "tkxmacro.hpp"

namespace acc::globals {
inline static const std::unordered_map<acc::GLOBAL_TOKENS, std::size_t> prec_map{
    {acc::TK_COMMA, 0},
    {acc::TK_EQUALS, 1},
    {acc::TK_LT, 2},
    {acc::TK_GT, 2},
    {acc::TK_GT_EQ, 2},
    {acc::TK_LT_EQ, 2},
    {acc::TK_PLUS, 3},
    {acc::TK_DASH, 3},
    {acc::TK_SLASH, 4},
    {acc::TK_STAR, 4},

};

}