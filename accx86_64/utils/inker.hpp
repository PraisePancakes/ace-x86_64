#pragma once
#include <iostream>

namespace acc::utils {

struct inker {
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

    void print_depth(std::size_t N, const char c) const noexcept {
        while (N--) {
            std::cout << c;
        }
    }

    std::ostream& os;
    inker(std::ostream& os) : os(os) {};
    ~inker() = default;
};

}  // namespace acc::utils