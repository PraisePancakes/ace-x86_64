#pragma once
#include <iostream>
namespace acc::types {
enum class TYPES : std::uint64_t {
    BOOL = 0,
    CHAR = 1,
    SHORT = 2,
    INT = 3,
    LONG = 4,
    LONG_LONG = 5,
    FLOAT = 6,
    DOUBLE = 7
};

}