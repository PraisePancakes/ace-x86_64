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

parser<char> match_(const char c) {
    return [=](std::istream& s) -> result<char> {
        char v = s.get();
        if (v == c) {
            return v;
        } else {
            s.unget();
            return std::unexpected("char_ error");
        }
    };
}

parser<int> digit_() {
    return [](std::istream& s) -> result<int> {
        char v = s.get();
        if (std::isdigit((unsigned char)v)) {
            int x = v - '0';
            return x;
        } else {
            s.unget();
            return std::unexpected("digit_ error");
        }
    };
};

parser<int> int_() {
    return [](std::istream& ss) -> result<int> {
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

        if (ret.empty()) return std::unexpected("number_ error");
        return std::stoi(ret);
    };
};

parser<char> letter_() {
    return [=](std::istream& ss) -> result<char> {
        auto v = ss.get();
        if (std::isalpha(v)) {
            return v;
        } else {
            ss.unget();
            return std::unexpected("letter_ error");
        }
    };
};

parser<std::string> letters_() {
    return [](std::istream& ss) -> result<std::string> {
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
        if (ret.empty()) return std::unexpected("letters_ error");
        return ret;
    };
}

parser<std::string> match_(std::string s) {
    return [=](std::istream& ss) -> result<std::string> {
        std::string ret = "";
        for (std::size_t i = 0; i < s.size(); i++) {
            auto sv = s[i];
            auto char_parser = match_(sv);
            auto v = char_parser(ss);
            if (v.has_value()) {
                ret += v.value();
            }
            if (ret == s) {
                break;
            }
        }

        if (ret != s) return std::unexpected("match_ error");
        return ret;
    };
};

// many_match_parser = many_(match_("hello"));
template <typename Ts>
[[nodiscard]] parser<std::vector<Ts>> many_(const parser<Ts>& ps) {
    return [ps](std::istream& ss) -> result<std::vector<Ts>> {
        std::vector<Ts> parsed;
        while (true) {
            auto ir = ps(ss);
            if (!ir.has_value()) {
                break;
            }
            parsed.push_back(ir.value());
        }
        if (parsed.empty()) return std::unexpected("many_ found none");
        return parsed;
    };
};

}  // namespace acc