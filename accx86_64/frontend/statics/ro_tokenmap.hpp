#pragma once
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <variant>

#include "tkxmacro.hpp"

namespace acc::globals {
constexpr std::uint8_t TOKEN_TYPE_SHIFTER = 5;

enum token_flags_ : std::uint8_t {
    FLAG_DELIM = 1 << 0,
    FLAG_RESERVED = 1 << 1,
    FLAG_RESERVED_TYPE = 1 << 2,
    FLAG_DELIM_PAIR = 1 << 3,
    FLAG_BINARY_OP = 1 << 4
};

const static std::unordered_map<std::variant<std::string, char>, std::uint64_t> token_map{
    {'\"', ((acc::GLOBAL_TOKENS::TK_QUOTE_DOUBLE << TOKEN_TYPE_SHIFTER) | FLAG_DELIM)},
    {'(', ((acc::GLOBAL_TOKENS::TK_PAREN_L << TOKEN_TYPE_SHIFTER) | FLAG_DELIM)},
    {')', ((acc::GLOBAL_TOKENS::TK_PAREN_R << TOKEN_TYPE_SHIFTER) | FLAG_DELIM)},
    {',', ((acc::GLOBAL_TOKENS::TK_COMMA << TOKEN_TYPE_SHIFTER) | FLAG_DELIM)},
    {':', ((acc::GLOBAL_TOKENS::TK_COLON << TOKEN_TYPE_SHIFTER) | FLAG_DELIM)},
    {'[', ((acc::GLOBAL_TOKENS::TK_BRACE_L << TOKEN_TYPE_SHIFTER) | FLAG_DELIM)},
    {']', ((acc::GLOBAL_TOKENS::TK_BRACE_R << TOKEN_TYPE_SHIFTER) | FLAG_DELIM)},
    {'$', ((acc::GLOBAL_TOKENS::TK_DOLLA << TOKEN_TYPE_SHIFTER) | FLAG_DELIM)},
    {'{', ((acc::GLOBAL_TOKENS::TK_CURL_L << TOKEN_TYPE_SHIFTER) | FLAG_DELIM)},
    {'}', ((acc::GLOBAL_TOKENS::TK_CURL_R << TOKEN_TYPE_SHIFTER) | FLAG_DELIM)},
    {'\'', ((acc::GLOBAL_TOKENS::TK_QUOTE_SINGLE << TOKEN_TYPE_SHIFTER) | FLAG_DELIM)},
    {'+', ((acc::GLOBAL_TOKENS::TK_PLUS << TOKEN_TYPE_SHIFTER) | FLAG_DELIM | FLAG_BINARY_OP)},
    {'*', ((acc::GLOBAL_TOKENS::TK_STAR << TOKEN_TYPE_SHIFTER) | FLAG_DELIM | FLAG_BINARY_OP)},
    {'/', ((acc::GLOBAL_TOKENS::TK_SLASH << TOKEN_TYPE_SHIFTER) | FLAG_DELIM | FLAG_BINARY_OP)},
    {'-', ((acc::GLOBAL_TOKENS::TK_DASH << TOKEN_TYPE_SHIFTER) | FLAG_DELIM | FLAG_BINARY_OP)},
    {'.', ((acc::GLOBAL_TOKENS::TK_DOT << TOKEN_TYPE_SHIFTER) | FLAG_DELIM)},
    {'!', ((acc::GLOBAL_TOKENS::TK_BANG << TOKEN_TYPE_SHIFTER) | FLAG_DELIM)},
    {'=', ((acc::GLOBAL_TOKENS::TK_EQUALS << TOKEN_TYPE_SHIFTER) | FLAG_DELIM | FLAG_BINARY_OP)},
    {' ', ((acc::GLOBAL_TOKENS::TK_SPACE << TOKEN_TYPE_SHIFTER) | FLAG_DELIM)},
    {'<', ((acc::GLOBAL_TOKENS::TK_LT << TOKEN_TYPE_SHIFTER) | FLAG_DELIM | FLAG_BINARY_OP)},
    {'>', ((acc::GLOBAL_TOKENS::TK_GT << TOKEN_TYPE_SHIFTER) | FLAG_DELIM | FLAG_BINARY_OP)},
    {';', ((acc::GLOBAL_TOKENS::TK_SEMI << TOKEN_TYPE_SHIFTER) | FLAG_DELIM)},
    {'\n', ((acc::GLOBAL_TOKENS::TK_NEWLINE << TOKEN_TYPE_SHIFTER) | FLAG_DELIM)},

    {"!=", ((acc::GLOBAL_TOKENS::TK_BANG_EQ << TOKEN_TYPE_SHIFTER) | FLAG_DELIM_PAIR | FLAG_BINARY_OP)},
    {"==", ((acc::GLOBAL_TOKENS::TK_STRICT_EQ << TOKEN_TYPE_SHIFTER) | FLAG_DELIM_PAIR | FLAG_BINARY_OP)},
    {">=", ((acc::GLOBAL_TOKENS::TK_GT_EQ << TOKEN_TYPE_SHIFTER) | FLAG_DELIM_PAIR | FLAG_BINARY_OP)},
    {"<=", ((acc::GLOBAL_TOKENS::TK_LT_EQ << TOKEN_TYPE_SHIFTER) | FLAG_DELIM_PAIR | FLAG_BINARY_OP)},

    {(char)0xFAull, ((acc::GLOBAL_TOKENS::TK_LITERAL_DOUBLE << TOKEN_TYPE_SHIFTER))},
    {(char)0xFBull, ((acc::GLOBAL_TOKENS::TK_LITERAL_STRING << TOKEN_TYPE_SHIFTER))},
    {(char)0xFCull, ((acc::GLOBAL_TOKENS::TK_IDENTIFIER << TOKEN_TYPE_SHIFTER))},
    {(char)0xFDull, ((acc::GLOBAL_TOKENS::TK_LITERAL_FLOAT << TOKEN_TYPE_SHIFTER))},
    {(char)0xFEull, ((acc::GLOBAL_TOKENS::TK_LITERAL_INT << TOKEN_TYPE_SHIFTER))},
    {(char)0xFFull, ((acc::GLOBAL_TOKENS::TK_UNKNOWN << TOKEN_TYPE_SHIFTER))},

    // --- Reserved types ---
    {"int", ((acc::GLOBAL_TOKENS::TK_RESERVED_TYPE << TOKEN_TYPE_SHIFTER) | FLAG_RESERVED_TYPE | FLAG_RESERVED)},
    {"float", ((acc::GLOBAL_TOKENS::TK_RESERVED_TYPE << TOKEN_TYPE_SHIFTER) | FLAG_RESERVED_TYPE | FLAG_RESERVED)},
    {"char", ((acc::GLOBAL_TOKENS::TK_RESERVED_TYPE << TOKEN_TYPE_SHIFTER) | FLAG_RESERVED_TYPE | FLAG_RESERVED)},
    {"bool", ((acc::GLOBAL_TOKENS::TK_RESERVED_TYPE << TOKEN_TYPE_SHIFTER) | FLAG_RESERVED_TYPE | FLAG_RESERVED)},
    {"short", ((acc::GLOBAL_TOKENS::TK_RESERVED_TYPE << TOKEN_TYPE_SHIFTER) | FLAG_RESERVED_TYPE | FLAG_RESERVED)},
    {"long", ((acc::GLOBAL_TOKENS::TK_RESERVED_TYPE << TOKEN_TYPE_SHIFTER) | FLAG_RESERVED_TYPE | FLAG_RESERVED)},
    {"double", ((acc::GLOBAL_TOKENS::TK_RESERVED_TYPE << TOKEN_TYPE_SHIFTER) | FLAG_RESERVED_TYPE | FLAG_RESERVED)},

    // --- Reserved keywords ---
    {"mut", ((acc::GLOBAL_TOKENS::TK_RESERVED << TOKEN_TYPE_SHIFTER) | FLAG_RESERVED)},
    {"if", ((acc::GLOBAL_TOKENS::TK_RESERVED << TOKEN_TYPE_SHIFTER) | FLAG_RESERVED)},
    {"else", ((acc::GLOBAL_TOKENS::TK_RESERVED << TOKEN_TYPE_SHIFTER) | FLAG_RESERVED)},
    {"while", ((acc::GLOBAL_TOKENS::TK_RESERVED << TOKEN_TYPE_SHIFTER) | FLAG_RESERVED)},
    {"for", ((acc::GLOBAL_TOKENS::TK_RESERVED << TOKEN_TYPE_SHIFTER) | FLAG_RESERVED)},
    {"volatile", ((acc::GLOBAL_TOKENS::TK_RESERVED << TOKEN_TYPE_SHIFTER) | FLAG_RESERVED)}};

struct lexeme_inspector {
    [[nodiscard]] static acc::GLOBAL_TOKENS to_type(std::variant<std::string, char> key) {
        return acc::GLOBAL_TOKENS(token_map.find(key)->second >> TOKEN_TYPE_SHIFTER);
    };

    [[nodiscard]] static bool is_reserved(std::string keyword) noexcept {
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

    [[nodiscard]] static bool is_binary_op(const char unit) noexcept {
        if (token_map.find(unit) == token_map.end()) return false;
        return (((token_map.find(unit))->second & acc::globals::token_flags_::FLAG_BINARY_OP) == acc::globals::token_flags_::FLAG_BINARY_OP);
    }

    [[nodiscard]] static bool is_binary_op(const std::string& binop) {
        if (token_map.find(binop) == token_map.end()) return false;
        return (((token_map.find(binop))->second & acc::globals::token_flags_::FLAG_BINARY_OP) == acc::globals::token_flags_::FLAG_BINARY_OP);
    }

    [[nodiscard]] static bool is_binary_op(const acc::token token) {
        if (token.word.size() <= 1) return is_binary_op(token.word[0]);
        return is_binary_op(token.word);
    }
};

}  // namespace acc::globals
