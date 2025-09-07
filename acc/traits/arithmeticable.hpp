#pragma once
#include <iostream>

namespace acc::traits {
template <typename T>
concept arithmeticable = std::is_arithmetic_v<T>;
}