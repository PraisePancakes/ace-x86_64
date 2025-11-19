#pragma once
#include <iostream>

namespace acc::utils {

struct color_printer {
    template <typename... Args>
    void print_green(Args&&... args) noexcept {
        (((os << acc::ansi::foreground_green << args << acc::ansi::reset), ...) << "\n");
    }
    template <typename... Args>
    void print_yellow(Args&&... args) noexcept {
        (((os << acc::ansi::foreground_yellow << args << acc::ansi::reset), ...) << std::endl);
    }
    template <typename... Args>
    void print_red(Args&&... args) noexcept {
        (((os << acc::ansi::foreground_light_red << args << acc::ansi::reset), ...) << std::endl);
    }

    template <typename... Args>
    void print_blue(Args&&... args) noexcept {
        (((os << acc::ansi::foreground_blue << args << acc::ansi::reset), ...) << std::endl);
    }
    std::ostream& os;
    color_printer(std::ostream& os) : os(os) {};
    ~color_printer() = default;
};

}  // namespace acc::utils