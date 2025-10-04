#pragma once
#include <iostream>

#include "../frontend/acctoken.hpp"
namespace acc::traits {
template <typename T>
concept acc_matchable = std::is_same_v<T, acc::GLOBAL_TOKENS> || std::is_same_v<T, std::string> || std::is_same_v<std::decay_t<T>, const char*>;
}  // namespace acc::traits