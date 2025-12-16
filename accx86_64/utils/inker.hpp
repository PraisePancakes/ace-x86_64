#pragma once
#include <iostream>

namespace acc::utils {

struct inker {
    template <typename... Args>
    inker& print_green(Args&&... args) noexcept {
        (((os << acc::ansi::foreground_green << args << acc::ansi::reset), ...) << "\n");
        return *this;
    }
    template <typename... Args>
    inker& print_yellow(Args&&... args) noexcept {
        (((os << acc::ansi::foreground_yellow << args << acc::ansi::reset), ...) << "\n");
        return *this;
    }
    template <typename... Args>
    inker& print_red(Args&&... args) noexcept {
        (((os << acc::ansi::foreground_light_red << args << acc::ansi::reset), ...) << "\n");
        return *this;
    }

    template <typename... Args>
    inker& print_blue(Args&&... args) noexcept {
        (((os << acc::ansi::foreground_blue << args << acc::ansi::reset), ...) << "\n");
        return *this;
    }

    inker& print_depth(std::size_t N, const char c) noexcept {
        while (N--) {
            os << c;
        }
        return *this;
    }

    std::ostream& os;
    inker(std::ostream& os) : os(os) {};
    ~inker() = default;
};

}  // namespace acc::utils