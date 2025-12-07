#pragma once
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <variant>

#include "tkxmacro.hpp"

namespace acc::globals {
static constexpr std::uint8_t TOKEN_TYPE_SHIFTER = 5;
enum token_flags_ : std::uint8_t {
    FLAG_NULL = 0,
    FLAG_DELIM = 1 << 0,
    FLAG_RESERVED = 1 << 1,
    FLAG_RESERVED_TYPE = 1 << 2,
    FLAG_DELIM_PAIR = 1 << 3,
    FLAG_BINARY_OP = 1 << 4
};

template <typename... TOKEN_FLAGS>
    requires((std::is_enum_v<TOKEN_FLAGS> &&
              std::is_same_v<std::underlying_type_t<TOKEN_FLAGS>, std::uint8_t>) &&
             ...)
auto MAKE_TKREP(acc::GLOBAL_TOKENS Type, TOKEN_FLAGS... args) noexcept {
    return (Type << TOKEN_TYPE_SHIFTER) | (args | ...);
};

const static std::unordered_map<std::variant<std::string, char>, std::uint64_t> token_map{
    {'\"', MAKE_TKREP(TK_QUOTE_DOUBLE, FLAG_DELIM)},
    {'(', MAKE_TKREP(TK_PAREN_L, FLAG_DELIM)},
    {')', MAKE_TKREP(TK_PAREN_R, FLAG_DELIM)},
    {',', MAKE_TKREP(TK_COMMA, FLAG_DELIM, FLAG_BINARY_OP)},
    {':', MAKE_TKREP(TK_COLON, FLAG_DELIM)},
    {'[', MAKE_TKREP(TK_BRACE_L, FLAG_DELIM)},
    {']', MAKE_TKREP(TK_BRACE_R, FLAG_DELIM)},
    {'$', MAKE_TKREP(TK_DOLLA, FLAG_DELIM)},
    {'{', MAKE_TKREP(TK_CURL_L, FLAG_DELIM)},
    {'}', MAKE_TKREP(TK_CURL_R, FLAG_DELIM)},
    {'\'', MAKE_TKREP(TK_QUOTE_SINGLE, FLAG_DELIM)},
    {'+', MAKE_TKREP(TK_PLUS, FLAG_DELIM, FLAG_BINARY_OP)},
    {'*', MAKE_TKREP(TK_STAR, FLAG_DELIM, FLAG_BINARY_OP)},
    {'/', MAKE_TKREP(TK_SLASH, FLAG_DELIM, FLAG_BINARY_OP)},
    {'-', MAKE_TKREP(TK_DASH, FLAG_DELIM, FLAG_BINARY_OP)},
    {'.', MAKE_TKREP(TK_DOT, FLAG_DELIM)},
    {'!', MAKE_TKREP(TK_BANG, FLAG_DELIM)},
    {'=', MAKE_TKREP(TK_EQUALS, FLAG_DELIM)},
    {' ', MAKE_TKREP(TK_SPACE, FLAG_DELIM)},
    {'<', MAKE_TKREP(TK_LT, FLAG_DELIM, FLAG_BINARY_OP)},
    {'>', MAKE_TKREP(TK_GT, FLAG_DELIM, FLAG_BINARY_OP)},
    {';', MAKE_TKREP(TK_SEMI, FLAG_DELIM)},
    {'\n', MAKE_TKREP(TK_NEWLINE, FLAG_DELIM)},

    // --- 2-char operators ---
    {":=", MAKE_TKREP(TK_ASSIGNMENT, FLAG_DELIM_PAIR, FLAG_BINARY_OP)},
    {"!=", MAKE_TKREP(TK_BANG_EQ, FLAG_DELIM_PAIR, FLAG_BINARY_OP)},
    {"==", MAKE_TKREP(TK_STRICT_EQ, FLAG_DELIM_PAIR, FLAG_BINARY_OP)},
    {">=", MAKE_TKREP(TK_GT_EQ, FLAG_DELIM_PAIR, FLAG_BINARY_OP)},
    {"<=", MAKE_TKREP(TK_LT_EQ, FLAG_DELIM_PAIR, FLAG_BINARY_OP)},

    // --- Internal single-byte lexeme class markers ---
    {(char)0xF7ull, MAKE_TKREP(TK_LITERAL_BOOL, FLAG_NULL)},
    {(char)0xF8ull, MAKE_TKREP(TK_LITERAL_CHAR, FLAG_NULL)},
    {(char)0xF9ull, MAKE_TKREP(TK_LITERAL_SHORT, FLAG_NULL)},
    {(char)0xFAull, MAKE_TKREP(TK_LITERAL_INT, FLAG_NULL)},
    {(char)0xFBull, MAKE_TKREP(TK_LITERAL_LONG, FLAG_NULL)},
    {(char)0xFCull, MAKE_TKREP(TK_LITERAL_LONG_LONG, FLAG_NULL)},
    {(char)0xFDull, MAKE_TKREP(TK_LITERAL_FLOAT, FLAG_NULL)},
    {(char)0xFEull, MAKE_TKREP(TK_LITERAL_DOUBLE, FLAG_NULL)},
    {(char)0xFFull, MAKE_TKREP(TK_IDENTIFIER, FLAG_NULL)},

    // --- Reserved types ---
    {"int", MAKE_TKREP(TK_RESERVED_TYPE, FLAG_RESERVED_TYPE, FLAG_RESERVED)},
    {"float", MAKE_TKREP(TK_RESERVED_TYPE, FLAG_RESERVED_TYPE, FLAG_RESERVED)},
    {"char", MAKE_TKREP(TK_RESERVED_TYPE, FLAG_RESERVED_TYPE, FLAG_RESERVED)},
    {"bool", MAKE_TKREP(TK_RESERVED_TYPE, FLAG_RESERVED_TYPE, FLAG_RESERVED)},
    {"short", MAKE_TKREP(TK_RESERVED_TYPE, FLAG_RESERVED_TYPE, FLAG_RESERVED)},
    {"long", MAKE_TKREP(TK_RESERVED_TYPE, FLAG_RESERVED_TYPE, FLAG_RESERVED)},
    {"double", MAKE_TKREP(TK_RESERVED_TYPE, FLAG_RESERVED_TYPE, FLAG_RESERVED)},

    // --- Reserved keywords ---
    {"mut", MAKE_TKREP(TK_RESERVED, FLAG_RESERVED)},
    {"return", MAKE_TKREP(TK_RESERVED, FLAG_RESERVED)},
    {"if", MAKE_TKREP(TK_RESERVED, FLAG_RESERVED)},
    {"else", MAKE_TKREP(TK_RESERVED, FLAG_RESERVED)},
    {"while", MAKE_TKREP(TK_RESERVED, FLAG_RESERVED)},
    {"for", MAKE_TKREP(TK_RESERVED, FLAG_RESERVED)},
    {"volatile", MAKE_TKREP(TK_RESERVED, FLAG_RESERVED)},
};

struct lexeme_inspector {
    [[nodiscard]] static acc::GLOBAL_TOKENS to_kind(std::variant<std::string, char> key) {
        return acc::GLOBAL_TOKENS(token_map.find(key)->second >> TOKEN_TYPE_SHIFTER);
    };

    [[nodiscard]] static bool
    is_reserved(std::string keyword) noexcept {
        if (token_map.find(keyword) == token_map.end()) return false;
        return ((token_map.find(keyword)->second & acc::globals::token_flags_::FLAG_RESERVED) == acc::globals::token_flags_::FLAG_RESERVED);
    }

    [[nodiscard]] static bool is_type(std::string type) noexcept {
        if (token_map.find(type) == token_map.end()) return false;
        return ((token_map.find(type)->second & acc::globals::token_flags_::FLAG_RESERVED_TYPE) == acc::globals::token_flags_::FLAG_RESERVED_TYPE);
    }

    [[nodiscard]] static bool is_pair_delim(const char unit1, const char unit2) noexcept {
        if (token_map.find(std::string({unit1, unit2})) == token_map.end()) return false;
        return ((token_map.find(std::string({unit1, unit2}))->second & acc::globals::token_flags_::FLAG_DELIM_PAIR) == acc::globals::token_flags_::FLAG_DELIM_PAIR);
    }

    [[nodiscard]] static bool is_delim(const char unit) noexcept {
        if (token_map.find(unit) == token_map.end()) return false;
        return (((token_map.find(unit))->second & acc::globals::token_flags_::FLAG_DELIM) == acc::globals::token_flags_::FLAG_DELIM);
    }

    [[nodiscard]] static bool is_binary_op(const char unit, bool in_params = false) noexcept {
        if (token_map.find(unit) == token_map.end()) return false;
        if (unit == ',') {
            if (in_params) return false;
        };
        return (((token_map.find(unit))->second & acc::globals::token_flags_::FLAG_BINARY_OP) == acc::globals::token_flags_::FLAG_BINARY_OP);
    }

    [[nodiscard]] static bool is_binary_op(const std::string& binop) {
        if (token_map.find(binop) == token_map.end()) return false;
        return (((token_map.find(binop))->second & acc::globals::token_flags_::FLAG_BINARY_OP) == acc::globals::token_flags_::FLAG_BINARY_OP);
    }

    [[nodiscard]] static bool is_binary_op(const acc::token token, bool in_params = false) {
        if (token.word.size() <= 1) return is_binary_op(token.word[0], in_params);
        return is_binary_op(token.word);
    }
};

}  // namespace acc::globals
