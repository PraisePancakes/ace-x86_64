#pragma once
#include <iostream>

#include "../frontend/acctoken.hpp"
namespace acc::traits {
template <typename T>
concept acc_token_t = std::is_same_v<T, acc::GLOBAL_TOKENS>;
}  // namespace acc::traits