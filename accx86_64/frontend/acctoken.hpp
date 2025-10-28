#pragma once
#include <iostream>
#include <variant>

#include "../utils/visitor.hpp"
#include "statics/tkxmacro.hpp"
namespace acc {

struct [[nodiscard]] token {
    using value_type = std::variant<char, int, float, std::string, double, bool>;

    std::string word;
    std::pair<int, int> location;
    using token_type_t = acc::GLOBAL_TOKENS;
    token_type_t type;
    value_type value;

    constexpr static inline const char* to_string(token_type_t type) noexcept {
        switch (type) {
#define TOKEN_DEF(NAME, VALUE) \
    case NAME:                 \
        return #NAME;
            TOKEN_DEFs
#undef TOKEN_DEF
                default : return "UNKNOWN_TYPE";
        }
    }

    constexpr static const char* to_stringized(token_type_t type) noexcept {
        switch (type) {
#define TOKEN_DEF(NAME, VALUE) \
    case NAME:                 \
        return #VALUE;
            TOKEN_DEFs
#undef TOKEN_DEF
                default : return "UNKNOWN_LITERAL";
        }
    }

    constexpr static inline std::string to_literal(token_type_t type) noexcept {
        const char* lit = to_stringized(type);
        std::string ret{lit};
        return ret.substr(1, ret.size() - 2);
    }

    void print_token() const noexcept {
        std::cout << "TOKEN TYPE ID (" << to_literal(type) << ")"
                  << " [" << to_string(type) << "]\n";
        std::cout << "location (row, col) < " << location.first << " , " << location.second << " > ";
        std::visit(internal::visitor{
                       [](char c) { std::cout << "[CHAR] " << c << std::endl; },
                       [](std::string s) { std::cout << "[STRING] " << s << std::endl; },
                       [](int i) { std::cout << "[INT] " << i << std::endl; },
                       [](float f) { std::cout << "[FLOAT] " << f << std::endl; },
                       [](double d) { std::cout << "[DOUBLE] " << d << std::endl; },
                       [](bool b) { std::cout << "[BOOL] " << std::boolalpha << b << std::endl; }},
                   value);
    };

    token() = default;
    token(const std::string& word, token_type_t type, value_type val) : word(word), location(0, 0), type(type), value(val) {};
    token(const std::string& word, std::pair<int, int> l, token_type_t type, value_type val) : word(word), location(l), type(type), value(val) {};
    token(const std::string& word, int x, int y, token_type_t type, value_type val) : word(word), location(std::make_pair(x, y)), type(type), value(val) {};
    token(const token& o) : word(o.word), location(o.location), type(o.type), value(o.value) {};
    token(token&& o) : word(std::move(o.word)), location(o.location), type(o.type), value(std::move(o.value)) {};
    token& operator=(const token& o) {
        this->word = o.word;
        this->location = o.location;
        this->type = o.type;
        return *this;
    };
    ~token() = default;
};
}  // namespace acc