#pragma once
#include <cstdint>
#include <iostream>
#include <optional>
#include <unordered_map>
#include <variant>

#include "../analysis/typesys/types.hpp"
#include "tkxmacro.hpp"

namespace acc::globals {
constexpr std::uint8_t TOKEN_TYPE_SHIFTER = 5;
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
auto MAKE_FLAG_REP(acc::GLOBAL_TOKENS Type, TOKEN_FLAGS... args) noexcept {
    return (Type << TOKEN_TYPE_SHIFTER) | (args | ...);
};

struct meta_info {
    std::uint64_t flags;
    std::uint64_t type;
    std::optional<acc::types::TYPES> deduced_type;
};

inline meta_info METAFY_TKREP(std::uint64_t flags) {
    return meta_info{
        .flags = flags, .type = flags >> TOKEN_TYPE_SHIFTER, .deduced_type = std::nullopt};
};

inline meta_info METAFY_TKREP(std::uint64_t flags, std::optional<acc::types::TYPES> deduced_type) {
    return meta_info{.flags = flags, .type = flags >> TOKEN_TYPE_SHIFTER, .deduced_type = deduced_type};
};
// METAFY_TKREP(MAKE_FLAG_REP(TK_QUOTE_DOUBLE, FLAG_DELIM))
const static std::unordered_map<std::variant<std::string, char>, meta_info>
    token_map{

        // --- 1-char delimiters, operators ---
        {'\"', METAFY_TKREP(MAKE_FLAG_REP(TK_QUOTE_DOUBLE, FLAG_DELIM))},
        {'(', METAFY_TKREP(MAKE_FLAG_REP(TK_PAREN_L, FLAG_DELIM))},
        {')', METAFY_TKREP(MAKE_FLAG_REP(TK_PAREN_R, FLAG_DELIM))},
        {',', METAFY_TKREP(MAKE_FLAG_REP(TK_COMMA, FLAG_DELIM, FLAG_BINARY_OP))},
        {':', METAFY_TKREP(MAKE_FLAG_REP(TK_COLON, FLAG_DELIM))},
        {'[', METAFY_TKREP(MAKE_FLAG_REP(TK_BRACE_L, FLAG_DELIM))},
        {']', METAFY_TKREP(MAKE_FLAG_REP(TK_BRACE_R, FLAG_DELIM))},
        {'$', METAFY_TKREP(MAKE_FLAG_REP(TK_DOLLA, FLAG_DELIM))},
        {'{', METAFY_TKREP(MAKE_FLAG_REP(TK_CURL_L, FLAG_DELIM))},
        {'}', METAFY_TKREP(MAKE_FLAG_REP(TK_CURL_R, FLAG_DELIM))},
        {'\'', METAFY_TKREP(MAKE_FLAG_REP(TK_QUOTE_SINGLE, FLAG_DELIM))},
        {'+', METAFY_TKREP(MAKE_FLAG_REP(TK_PLUS, FLAG_DELIM, FLAG_BINARY_OP))},
        {'*', METAFY_TKREP(MAKE_FLAG_REP(TK_STAR, FLAG_DELIM, FLAG_BINARY_OP))},
        {'/', METAFY_TKREP(MAKE_FLAG_REP(TK_SLASH, FLAG_DELIM, FLAG_BINARY_OP))},
        {'-', METAFY_TKREP(MAKE_FLAG_REP(TK_DASH, FLAG_DELIM, FLAG_BINARY_OP))},
        {'.', METAFY_TKREP(MAKE_FLAG_REP(TK_DOT, FLAG_DELIM))},
        {'!', METAFY_TKREP(MAKE_FLAG_REP(TK_BANG, FLAG_DELIM))},
        {'=', METAFY_TKREP(MAKE_FLAG_REP(TK_EQUALS, FLAG_DELIM))},
        {' ', METAFY_TKREP(MAKE_FLAG_REP(TK_SPACE, FLAG_DELIM))},
        {'<', METAFY_TKREP(MAKE_FLAG_REP(TK_LT, FLAG_DELIM, FLAG_BINARY_OP))},
        {'>', METAFY_TKREP(MAKE_FLAG_REP(TK_GT, FLAG_DELIM, FLAG_BINARY_OP))},
        {';', METAFY_TKREP(MAKE_FLAG_REP(TK_SEMI, FLAG_DELIM))},
        {'\n', METAFY_TKREP(MAKE_FLAG_REP(TK_NEWLINE, FLAG_DELIM))},

        // --- 2-char operators ---
        {":=", METAFY_TKREP(MAKE_FLAG_REP(TK_ASSIGNMENT, FLAG_DELIM_PAIR, FLAG_BINARY_OP))},
        {"!=", METAFY_TKREP(MAKE_FLAG_REP(TK_BANG_EQ, FLAG_DELIM_PAIR, FLAG_BINARY_OP))},
        {"==", METAFY_TKREP(MAKE_FLAG_REP(TK_STRICT_EQ, FLAG_DELIM_PAIR, FLAG_BINARY_OP))},
        {">=", METAFY_TKREP(MAKE_FLAG_REP(TK_GT_EQ, FLAG_DELIM_PAIR, FLAG_BINARY_OP))},
        {"<=", METAFY_TKREP(MAKE_FLAG_REP(TK_LT_EQ, FLAG_DELIM_PAIR, FLAG_BINARY_OP))},

        // --- Internal single-byte literal classes ---
        {(char)0xF7ull, METAFY_TKREP(MAKE_FLAG_REP(TK_LITERAL_BOOL, FLAG_NULL),
                                     acc::types::TYPES::BOOL)},
        {(char)0xF8ull, METAFY_TKREP(MAKE_FLAG_REP(TK_LITERAL_CHAR, FLAG_NULL),
                                     acc::types::TYPES::CHAR)},
        {(char)0xF9ull, METAFY_TKREP(MAKE_FLAG_REP(TK_LITERAL_SHORT, FLAG_NULL),
                                     acc::types::TYPES::SHORT)},
        {(char)0xFAull, METAFY_TKREP(MAKE_FLAG_REP(TK_LITERAL_INT, FLAG_NULL),
                                     acc::types::TYPES::INT)},
        {(char)0xFBull, METAFY_TKREP(MAKE_FLAG_REP(TK_LITERAL_LONG, FLAG_NULL),
                                     acc::types::TYPES::LONG)},
        {(char)0xFCull, METAFY_TKREP(MAKE_FLAG_REP(TK_LITERAL_LONG_LONG, FLAG_NULL),
                                     acc::types::TYPES::LONG_LONG)},
        {(char)0xFDull, METAFY_TKREP(MAKE_FLAG_REP(TK_LITERAL_FLOAT, FLAG_NULL),
                                     acc::types::TYPES::FLOAT)},
        {(char)0xFEull, METAFY_TKREP(MAKE_FLAG_REP(TK_LITERAL_DOUBLE, FLAG_NULL),
                                     acc::types::TYPES::DOUBLE)},
        {(char)0xFFull, METAFY_TKREP(MAKE_FLAG_REP(TK_IDENTIFIER, FLAG_NULL))},  // identifiers = unknown type initially

        // --- Reserved types (with deduced type) ---
        {"int", METAFY_TKREP(MAKE_FLAG_REP(TK_RESERVED_TYPE, FLAG_RESERVED_TYPE, FLAG_RESERVED),
                             acc::types::TYPES::INT)},
        {"float", METAFY_TKREP(MAKE_FLAG_REP(TK_RESERVED_TYPE, FLAG_RESERVED_TYPE, FLAG_RESERVED),
                               acc::types::TYPES::FLOAT)},
        {"char", METAFY_TKREP(MAKE_FLAG_REP(TK_RESERVED_TYPE, FLAG_RESERVED_TYPE, FLAG_RESERVED),
                              acc::types::TYPES::CHAR)},
        {"bool", METAFY_TKREP(MAKE_FLAG_REP(TK_RESERVED_TYPE, FLAG_RESERVED_TYPE, FLAG_RESERVED),
                              acc::types::TYPES::BOOL)},
        {"short", METAFY_TKREP(MAKE_FLAG_REP(TK_RESERVED_TYPE, FLAG_RESERVED_TYPE, FLAG_RESERVED),
                               acc::types::TYPES::SHORT)},
        {"long", METAFY_TKREP(MAKE_FLAG_REP(TK_RESERVED_TYPE, FLAG_RESERVED_TYPE, FLAG_RESERVED),
                              acc::types::TYPES::LONG)},
        {"double", METAFY_TKREP(MAKE_FLAG_REP(TK_RESERVED_TYPE, FLAG_RESERVED_TYPE, FLAG_RESERVED),
                                acc::types::TYPES::DOUBLE)},

        // --- Reserved keywords (no deduced type) ---
        {"mut", METAFY_TKREP(MAKE_FLAG_REP(TK_RESERVED, FLAG_RESERVED))},
        {"return", METAFY_TKREP(MAKE_FLAG_REP(TK_RESERVED, FLAG_RESERVED))},
        {"if", METAFY_TKREP(MAKE_FLAG_REP(TK_RESERVED, FLAG_RESERVED))},
        {"else", METAFY_TKREP(MAKE_FLAG_REP(TK_RESERVED, FLAG_RESERVED))},
        {"while", METAFY_TKREP(MAKE_FLAG_REP(TK_RESERVED, FLAG_RESERVED))},
        {"for", METAFY_TKREP(MAKE_FLAG_REP(TK_RESERVED, FLAG_RESERVED))},
        {"volatile", METAFY_TKREP(MAKE_FLAG_REP(TK_RESERVED, FLAG_RESERVED))},
        {"type", METAFY_TKREP(MAKE_FLAG_REP(TK_RESERVED, FLAG_RESERVED))}};

}  // namespace acc::globals
