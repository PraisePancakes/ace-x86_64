#pragma once
#include <iostream>
#include <variant>

#include "../utils/visitor.hpp"
#include "statics/tkxmacro.hpp"
namespace acc {

struct [[nodiscard]] token final {
    using value_type = std::variant<char, int, float, std::string, double, bool>;
    using token_type_t = acc::GLOBAL_TOKENS;
    std::string word;
    std::pair<int, int> location;

    token_type_t type;
    value_type value;

    token() = default;
    token(const std::string& word, token_type_t type, value_type val) : word(word), location(0, 0), type(type), value(val) {};
    token(const std::string& word, std::pair<int, int> l, token_type_t type, value_type val) : word(word), location(l), type(type), value(val) {};
    token(const std::string& word, int x, int y, token_type_t type, value_type val) : word(word), location(std::make_pair(x, y)), type(type), value(val) {};
    
    friend bool operator==(const token& lhs, const token& rhs) {
        return lhs.type == rhs.type;
    }
    ~token() = default;
};
}  // namespace acc