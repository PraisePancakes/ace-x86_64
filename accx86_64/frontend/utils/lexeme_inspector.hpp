
#pragma once
#include <iostream>

#include "../acctoken.hpp"
#include "../statics/ro_tokenmap.hpp"

namespace acc::utils {
struct lexeme_inspector {
    [[nodiscard]] static acc::GLOBAL_TOKENS to_kind(std::variant<std::string, char> key) {
        return acc::GLOBAL_TOKENS(acc::globals::token_map.find(key)->second.flags >> acc::globals::TOKEN_TYPE_SHIFTER);
    };

    [[nodiscard]] static bool
    is_reserved(std::string keyword) noexcept {
        if (acc::globals::token_map.find(keyword) == acc::globals::token_map.end()) return false;
        return ((acc::globals::token_map.find(keyword)->second.flags & acc::globals::token_flags_::FLAG_RESERVED) == acc::globals::token_flags_::FLAG_RESERVED);
    }

    [[nodiscard]] static bool is_type(std::string type) noexcept {
        if (acc::globals::token_map.find(type) == acc::globals::token_map.end()) return false;
        return ((acc::globals::token_map.find(type)->second.flags & acc::globals::token_flags_::FLAG_RESERVED_TYPE) == acc::globals::token_flags_::FLAG_RESERVED_TYPE);
    }

    [[nodiscard]] static bool is_pair_delim(const char unit1, const char unit2) noexcept {
        if (acc::globals::token_map.find(std::string({unit1, unit2})) == acc::globals::token_map.end()) return false;
        return ((acc::globals::token_map.find(std::string({unit1, unit2}))->second.flags & acc::globals::token_flags_::FLAG_DELIM_PAIR) == acc::globals::token_flags_::FLAG_DELIM_PAIR);
    }

    [[nodiscard]] static bool is_delim(const char unit) noexcept {
        if (acc::globals::token_map.find(unit) == acc::globals::token_map.end()) return false;
        return (((acc::globals::token_map.find(unit))->second.flags & acc::globals::token_flags_::FLAG_DELIM) == acc::globals::token_flags_::FLAG_DELIM);
    }

    [[nodiscard]] static bool is_binary_op(const char unit, bool in_params = false) noexcept {
        if (acc::globals::token_map.find(unit) == acc::globals::token_map.end()) return false;
        if (unit == ',') {
            if (in_params) return false;
        };
        return (((acc::globals::token_map.find(unit))->second.flags & acc::globals::token_flags_::FLAG_BINARY_OP) == acc::globals::token_flags_::FLAG_BINARY_OP);
    }

    [[nodiscard]] static bool is_binary_op(const std::string& binop) {
        if (acc::globals::token_map.find(binop) == acc::globals::token_map.end()) return false;
        return (((acc::globals::token_map.find(binop))->second.flags & acc::globals::token_flags_::FLAG_BINARY_OP) == acc::globals::token_flags_::FLAG_BINARY_OP);
    }

    [[nodiscard]] static bool is_binary_op(const acc::token token, bool in_params = false) {
        if (token.word.size() <= 1) return is_binary_op(token.word[0], in_params);
        return is_binary_op(token.word);
    }
};
}  // namespace acc::utils
