#pragma once
#include <iostream>
namespace acc::traits {
template <typename T>
concept appendable = requires(T t, T v) {
    { t += v } -> std::convertible_to<T>;
};
}  // namespace acc