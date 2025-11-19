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