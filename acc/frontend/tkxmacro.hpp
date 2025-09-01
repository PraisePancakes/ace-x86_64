#pragma once

namespace acc {

#define TOKEN_DEFs                     \
    TOKEN_DEF(TK_QUOTE_DOUBLE, '\"')   \
    TOKEN_DEF(TK_PAREN_L, '(')         \
    TOKEN_DEF(TK_PAREN_R, ')')         \
    TOKEN_DEF(TK_COMMA, ',')           \
    TOKEN_DEF(TK_COLON, ':')           \
    TOKEN_DEF(TK_BRACE_L, '[')         \
    TOKEN_DEF(TK_BRACE_R, ']')         \
    TOKEN_DEF(TK_DOLLA, '$')           \
    TOKEN_DEF(TK_CURL_L, '{')          \
    TOKEN_DEF(TK_CURL_R, '}')          \
    TOKEN_DEF(TK_QUOTE_SINGLE, '\'')   \
    TOKEN_DEF(TK_PLUS, '+')            \
    TOKEN_DEF(TK_STAR, '*')            \
    TOKEN_DEF(TK_SLASH, '/')           \
    TOKEN_DEF(TK_DASH, '-')            \
    TOKEN_DEF(TK_DOT, '.')             \
    TOKEN_DEF(TK_SPACE, ' ')           \
    TOKEN_DEF(TK_NEWLINE, '\n')        \
    TOKEN_DEF(TK_LITERAL_INT, 0xFE)    \
    TOKEN_DEF(TK_LITERAL_FLOAT, 0xFD)  \
    TOKEN_DEF(TK_IDENTIFIER, 0xFC)     \
    TOKEN_DEF(TK_LITERAL_STRING, 0xFB) \
    TOKEN_DEF(TK_LITERAL_DOUBLE, 0xFA) \
    TOKEN_DEF(TK_UNKNOWN, 0xFF)

/// Enum representing token types
#define TOKEN_DEF(NAME, VALUE) NAME = VALUE,
enum ACC_ALL_TOKEN_ENUM : unsigned char {
    TOKEN_DEFs
};
#undef TOKEN_DEF
}  // namespace acc
