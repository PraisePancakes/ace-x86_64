#pragma once
#include <iostream>

namespace acc {

template <typename... Fs>
struct visitor : Fs... {
    using Fs::operator()...;
};

template <typename... fs>
visitor(fs...) -> visitor<fs...>;

};  // namespace acc