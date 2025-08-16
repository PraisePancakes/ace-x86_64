#pragma once
#include <expected>
#include <functional>
#include <iostream>
#include <string>

namespace acc {

template <typename T>
using result = std::expected<T, std::string>;

template <typename T>
struct parser : std::function<result<T>(std::istream&)> {
    using std::function<result<T>(std::istream&)>::function;
    using value_type = T;
    constexpr parser(parser<T> (*ptr)()) : parser(ptr()) {}
};

parser<char> match_(const char c, const std::string& error_message) {
    return [=](std::istream& s) -> result<char> {
        char v = s.get();
        if (v == c) {
            return v;
        } else {
            s.unget();
            return std::unexpected(error_message);
        }
    };
}

parser<char> match_(const char c) {
    return match_(c, "match_ parser error");
}

parser<std::string> match_(std::string s, const std::string& error_message) {
    return [=](std::istream& ss) -> result<std::string> {
        for (std::size_t i = 0; i < s.size(); i++) {
            if (ss.peek() == s[i]) {
                ss.get();
            } else {
                while (i--) {
                    ss.unget();
                }
                return std::unexpected("match_ error");
            }
        }
        return s;
    };
};

parser<std::string> match_(std::string s) {
    return match_(s, "match_ parser error");
};

parser<int> digit_(const std::string& error_message) {
    return [=](std::istream& s) -> result<int> {
        char v = s.get();
        if (std::isdigit((unsigned char)v)) {
            int x = v - '0';
            return x;
        } else {
            s.unget();
            return std::unexpected(error_message);
        }
    };
}

parser<int> digit_() {
    return digit_("digit_ parser error");
};

parser<int> int_(const std::string& error_message) {
    return [=](std::istream& ss) -> result<int> {
        auto digit_parser = digit_();
        std::string ret = "";

        while (true) {
            auto v = digit_parser(ss);
            if (v.has_value() && !ss.eof()) {
                ret += ('0' + v.value());
            } else {
                break;
            }
        };

        if (ret.empty()) return std::unexpected(error_message);
        return std::stoi(ret);
    };
}

parser<int> int_() {
    return int_("int_ parser error");
};

parser<char> letter_(const std::string& error_message) {
    return [=](std::istream& ss) -> result<char> {
        auto v = ss.get();
        if (std::isalpha(v)) {
            return v;
        } else {
            ss.unget();
            return std::unexpected(error_message);
        }
    };
}

parser<char> letter_() {
    return letter_("letter_ parser error");
};

parser<std::string> letters_(const std::string& error_message) {
    return [=](std::istream& ss) -> result<std::string> {
        auto letter_parser = letter_();
        std::string ret = "";
        while (true) {
            auto v = letter_parser(ss);
            if (v.has_value()) {
                ret += v.value();
            } else {
                break;
            }
        }
        if (ret.empty()) return std::unexpected(error_message);
        return ret;
    };
}

parser<std::string> letters_() {
    return letters_("letters_ parser error");
}

template <typename Ts>
[[nodiscard]] parser<std::vector<Ts>> many_(const parser<Ts>& ps, const std::string& error_message) {
    return [&ps, error_message](std::istream& ss) -> result<std::vector<Ts>> {
        std::vector<Ts> parsed;
        while (true) {
            auto ir = ps(ss);
            if (!ir.has_value()) {
                break;
            }
            parsed.push_back(ir.value());
        }
        if (parsed.empty()) return std::unexpected(error_message);
        return parsed;
    };
}

// many_match_parser = many_(match_("hello"));
template <typename Ts>
[[nodiscard]] parser<std::vector<Ts>> many_(const parser<Ts>& ps) {
    return many_(ps, "many_ parser error : found none");
};
template <typename T>
parser<std::tuple<>> ignore_(const parser<T>& ps, const std::string& error_message) {
    return [&ps, error_message](std::istream& ss) -> result<std::tuple<>> {
        if (!ps(ss)) {
            return std::unexpected(error_message);
        }
        return std::make_tuple();
    };
}

template <typename T>
parser<std::tuple<>> ignore_(const parser<T>& ps) {
    return ignore_(ps, "ignore_ parser error : ignored none");
}

template <typename A>
auto constexpr operator|(result<A> const& lhs, result<A> const& rhs) {
    return lhs ? lhs : rhs;
}

template <typename T>
constexpr auto either_(result<T> const& lhs, result<T> const& rhs) {
    return lhs ? lhs : rhs;
};

template <typename T, typename... Ts>
constexpr auto any_(result<T> const& ps, Ts... ts) {
    return ps | any_(ts...);
};

// create any_ which will take a variadic parser<Ts>... and convert them to parsers, result will be any parser that succeeds else return unexpected.

}  // namespace acc