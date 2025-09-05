#pragma once
#include <iostream>

namespace acc::ansi {

constexpr std::ostream &reset(std::ostream &os) {
    return os << "\033[0m";
}

constexpr std::ostream &foreground_black(std::ostream &os) {
    return os << "\033[30m";
}

constexpr std::ostream &foreground_red(std::ostream &os) {
    return os << "\033[31m";
}

constexpr std::ostream &foreground_yellow(std::ostream &os) {
    return os << "\033[33m";
}

constexpr std::ostream &foreground_green(std::ostream &os) {
    return os << "\033[32m";
}

constexpr std::ostream &foreground_grey(std::ostream &os) {
    return os << "\033[37m";
}

constexpr std::ostream &foreground_blue(std::ostream &os) {
    return os << "\033[34m";
}

};  // namespace acc::ansi
