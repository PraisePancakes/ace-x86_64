#pragma once
#include <expected>
#include <functional>
#include <iostream>
#include <string>

#include "../traits/appendable.hpp"
namespace acc {

template <typename T>
struct result : std::expected<T, std::string> {
    using base = std::expected<T, std::string>;
    using base::base;
    using base::operator bool;
    using base::value;

    template <typename F>
        requires(std::is_invocable_v<F, T>)
    auto transform(F&& f) {
        return std::invoke(f, this->value());
    }
};

template <typename T>
struct parser : std::function<result<T>(std::istream&)> {
    using std::function<result<T>(std::istream&)>::function;
    using value_type = T;
    constexpr parser(parser<T> (*ptr)()) : parser(ptr()) {}
};

template <typename A>
auto constexpr operator|(result<A> const& lhs, result<A> const& rhs) {
    return lhs ? lhs : rhs;
}

template <typename T>
constexpr auto either_1(result<T> const& lhs, result<T> const& rhs) {
    return lhs ? lhs : rhs;
};

template <typename T>
constexpr auto either_2(result<T> const& lhs, result<T> const& rhs) {
    return rhs ? rhs : lhs;
};

template <typename T, typename U>
[[nodiscard]] constexpr auto both_(parser<T> const& lhs, parser<U> const& rhs) {
    return [=](std::istream& ss) -> result<std::tuple<T, U>> {
        if (auto v = lhs(ss)) {
            if (auto r = rhs(ss)) {
                return std::make_tuple(v.value(), r.value());
            }
        }
        return std::unexpected("Not both");
    };
};

template <typename T, typename U>
[[nodiscard]] constexpr auto operator>>(parser<T> const& lhs, parser<U> const& rhs) {
    return both_(lhs, rhs);
};

template <typename... Ts>
[[nodiscard]] constexpr auto sequ_(Ts&&... ps) {
    return [xt = std::make_tuple(ps...)](std::istream& ss) {
        using OutTupleT = std::tuple<typename std::decay_t<decltype(ps)>::value_type...>;
        return [&]<std::size_t... I>(std::index_sequence<I...>)
                   -> result<OutTupleT> {
            bool success = true;
            std::string error = "";
            OutTupleT out;
            ([&]<std::size_t idx>(std::integral_constant<std::size_t, idx>) {
                auto v = std::get<idx>(xt)(ss);
                if (!success) return;
                if (v) {
                    std::get<idx>(out) = *v;
                } else {
                    success = false;
                    error = v.error();
                }
            }(std::integral_constant<std::size_t, I>{}),
             ...);
            if (!success) return std::unexpected(error);
            return out;
        }(std::make_index_sequence<sizeof...(Ts)>{});
    };
};

// (A , B) -> result<A + B>

template <typename... Ts>
[[nodiscard]] constexpr auto any_(parser<Ts> const&... ts) {
    return [=](std::istream& ss) -> auto {
        return (ts(ss) | ...);
    };
};

parser<char> match_(const char c, const std::string& error_message) {
    return [=](std::istream& s) -> result<char> {
        char v = s.get();
        if (v == c) {
            return v;
        } else {
            s.unget();
            return std::unexpected(error_message + " got: " + (char)v);
        }
    };
}

parser<char> match_(const char c) {
    return match_(c, "match_ parser error expected " + c);
}

parser<std::string> match_(const std::string& s, const std::string& error_message) {
    return [=](std::istream& ss) -> result<std::string> {
        std::string outbuilder = "";
        std::streampos pos = ss.tellg();
        for (std::size_t i = 0; i < s.size(); ++i) {
            if (ss.peek() == s[i]) {
                ss.get();
            } else {
                ss.seekg(pos);
                for (std::size_t x = 0; x < s.size(); x++) {
                    outbuilder += ss.get();
                }
                ss.seekg(pos);
                std::string error = error_message + " got : " + outbuilder;
                return std::unexpected(error);
            }
        }

        return s;
    };
};

parser<std::string> match_(std::string s) {
    return match_(s, "match_ parser error expected " + s);
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
            } else if (auto v = match_('-')(ss)) {
                ret += v.value();
            } else {
                try {
                    int i = std::stoi(ret);
                    return i;
                } catch (const std::invalid_argument&) {
                    return std::unexpected(error_message);
                };
            }
        };
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

parser<std::string> alnum_(const std::string& error_message) {
    return [=](std::istream& ss) -> result<std::string> {
        std::string ret = "";
        while (true) {
            if (std::isalnum(ss.peek())) {
                ret += ss.get();
            } else {
                break;
            }
        }

        if (ret.empty()) return std::unexpected(error_message);
        return ret;
    };
}

parser<std::string> alnum_() {
    return alnum_("letters_ parser error");
}

template <typename T>
constexpr parser<std::pair<std::vector<T>, std::string>> many_(const parser<T>& prsr) {
    using pair_type = std::pair<std::vector<T>, std::string>;
    return [=](std::istream& ss) -> result<pair_type> {
        std::vector<T> rv;
        std::string rs;
        while (auto v = prsr(ss)) {
            rv.push_back(v.value());
            if constexpr (traits::appendable<T>) {
                rs += v.value();
            }
        }
        return std::make_pair(rv, rs);
    };
};

template <typename T>
constexpr parser<std::pair<std::vector<T>, std::string>>
many_1(const parser<T>& prsr, const std::string& error_message) noexcept {
    using pair_type = std::pair<std::vector<T>, std::string>;
    return [=](std::istream& ss) -> result<pair_type> {
        auto v = many_(prsr)(ss);
        if (v.has_value()) {
            if (v.value().first.empty() || v.value().second.empty()) return std::unexpected(error_message);
            return v.value();
        }

        return std::unexpected(error_message);
    };
};

template <typename Ts>
[[nodiscard]] parser<std::pair<std::vector<Ts>, std::string>> many_1(const parser<Ts>& ps) noexcept {
    return many_1(ps, "many_1 parser error : found none");
};

template <typename T, typename F>
[[nodiscard]] constexpr parser<std::invoke_result_t<F>>
transform_(parser<T> const& p, const F& f) noexcept(
    noexcept(std::is_nothrow_invocable_v<F>)) {
    using U = std::invoke_result_t<F>;
    return [=](std::istream& ss) -> result<U> {
        auto v = p(ss);
        if (v.has_value()) {
            return std::invoke(f);
        }
        return std::unexpected(v.error());
    };
}

template <typename T, typename F>
[[nodiscard]] constexpr parser<std::invoke_result_t<F, T>>
transform_(parser<T> const& p, const F& f) noexcept(
    noexcept(std::is_nothrow_invocable_v<F, T>)) {
    using U = std::invoke_result_t<F, T>;
    return [=](std::istream& ss) -> result<U> {
        auto v = p(ss);
        if (v.has_value()) {
            return std::invoke(f, v.value());
        }
        return std::unexpected(v.error());
    };
}

template <typename T>
parser<std::pair<char, std::tuple<>>> ignore_(const parser<T>& ps) {
    return ignore_(ps, "ignore_ parser error : ignored none");
}

template <typename T>
parser<std::pair<char, std::tuple<>>> ignore_(const parser<T>& ps, const std::string& error_message) {
    return [&ps, error_message](std::istream& ss) -> result<std::pair<char, std::tuple<>>> {
        if (!ps(ss)) {
            return std::unexpected(error_message);
        }
        return std::make_pair(ss.peek(), std::make_tuple());
    };
};

[[nodiscard]] static parser<bool> eof_() {
    return [=](std::istream& ss) -> result<bool> {
        if (ss.peek() == EOF) {
            return true;
        }
        return std::unexpected("not eof");
    };
};

// create any_ which will take a variadic parser<Ts>... and convert them to parsers, result will be any parser that succeeds else return unexpected.

}  // namespace acc
