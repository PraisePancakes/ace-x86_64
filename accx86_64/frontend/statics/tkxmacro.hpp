#pragma once
#include <cstdint>

namespace acc {

#define TOKEN_DEFs                        \
    TOKEN_DEF(TK_RESERVED, 0x1ull)        \
    TOKEN_DEF(TK_RESERVED_TYPE, 0x2ull)   \
    TOKEN_DEF(TK_QUOTE_DOUBLE, '\"')      \
    TOKEN_DEF(TK_PAREN_L, '(')            \
    TOKEN_DEF(TK_PAREN_R, ')')            \
    TOKEN_DEF(TK_COMMA, ',')              \
    TOKEN_DEF(TK_COLON, ':')              \
    TOKEN_DEF(TK_BRACE_L, '[')            \
    TOKEN_DEF(TK_BRACE_R, ']')            \
    TOKEN_DEF(TK_DOLLA, '$')              \
    TOKEN_DEF(TK_CURL_L, '{')             \
    TOKEN_DEF(TK_CURL_R, '}')             \
    TOKEN_DEF(TK_QUOTE_SINGLE, '\'')      \
    TOKEN_DEF(TK_PLUS, '+')               \
    TOKEN_DEF(TK_STAR, '*')               \
    TOKEN_DEF(TK_SLASH, '/')              \
    TOKEN_DEF(TK_DASH, '-')               \
    TOKEN_DEF(TK_DOT, '.')                \
    TOKEN_DEF(TK_BANG, '!')               \
    TOKEN_DEF(TK_EQUALS, '=')             \
    TOKEN_DEF(TK_SPACE, ' ')              \
    TOKEN_DEF(TK_LT, '<')                 \
    TOKEN_DEF(TK_GT, '>')                 \
    TOKEN_DEF(TK_SEMI, ';')               \
    TOKEN_DEF(TK_NEWLINE, '\n')           \
    TOKEN_DEF(TK_BANG_EQ, 0xF6ull)        \
    TOKEN_DEF(TK_STRICT_EQ, 0xF7ull)      \
    TOKEN_DEF(TK_GT_EQ, 0xF8ull)          \
    TOKEN_DEF(TK_LT_EQ, 0xF9ull)          \
    TOKEN_DEF(TK_LITERAL_DOUBLE, 0xFAull) \
    TOKEN_DEF(TK_LITERAL_STRING, 0xFBull) \
    TOKEN_DEF(TK_IDENTIFIER, 0xFCull)     \
    TOKEN_DEF(TK_LITERAL_FLOAT, 0xFDull)  \
    TOKEN_DEF(TK_LITERAL_INT, 0xFEull)    \
    TOKEN_DEF(TK_UNKNOWN, 0xFFull)

/// Enum representing token types
#define TOKEN_DEF(NAME, VALUE) NAME = VALUE,
enum GLOBAL_TOKENS : std::uint64_t {
    TOKEN_DEFs
};
#undef TOKEN_DEF
}  // namespace acc
