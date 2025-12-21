#pragma once
#include <array>
#include <iostream>

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
static bool check_valid_implicit_conversion(TYPES t1, TYPES t2) noexcept {
    return implicit_cast_table[std::to_underlying(t1)][std::to_underlying(t2)];
};
}  // namespace acc::types