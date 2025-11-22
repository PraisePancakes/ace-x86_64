#pragma once
#include <iostream>
#include <vector>

#include "../acctoken.hpp"

namespace acc::output {
class token_printer {
    using token_type_t = acc::GLOBAL_TOKENS;
    std::vector<acc::token> tokens;

   public:
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

    static void write_token(std::ostream& out, acc::token tok) noexcept {
        out << "TOKEN TYPE ID (" << to_literal(tok.type) << ")"
            << " [" << to_string(tok.type) << "]\n";
        out << "location (row, col) < " << tok.location.first << " , " << tok.location.second << " > ";
        std::visit(internal::visitor{
                       [&out](char c) { out << "[CHAR] " << c << std::endl; },
                       [&out](std::string s) { out << "[STRING] " << s << std::endl; },
                       [&out](int i) { out << "[INT] " << i << std::endl; },
                       [&out](float f) { out << "[FLOAT] " << f << std::endl; },
                       [&out](double d) { out << "[DOUBLE] " << d << std::endl; },
                       [&out](bool b) { out << "[BOOL] " << std::boolalpha << b << std::endl; }},
                   tok.value);
    };
    std::ostream& os;
    token_printer(std::ostream& os) : os(os) {};
    void dump(const std::vector<acc::token>& toks) {
        for (auto tk : toks) {
            write_token(os, tk);
        }
    };
};
}  // namespace acc