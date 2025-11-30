#pragma once
#include <array>
#include <iostream>
namespace acc::types {  //                                                  int, float, char, bool, short, long, double
const std::array<std::array<std::uint8_t, 7>, 7> cast_bit_table = {{/*int*/ {1, 1, 1, 1, 1, 1, 1},
                                                                    /*float*/ {1, 1, 1, 1, 1, 1, 1},
                                                                    /*char*/ {1, 0, 1, 1, 1, 1, 0},
                                                                    /*bool*/ {1, 0, 0, 1, 1, 0, 0},
                                                                    /*short*/ {1, 1, 1, 1, 1, 1, 1},
                                                                    /*long*/ {1, 1, 1, 1, 1, 1, 1},
                                                                    /*double*/ {1, 1, 1, 1, 1, 1, 1}}};
enum class TYPES {
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
   
};
}  // namespace acc::types