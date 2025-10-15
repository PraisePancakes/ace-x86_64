#pragma once

namespace acc {

#define TOKEN_DEFs                      \
    TOKEN_DEF(TK_QUOTE_DOUBLE, '\"')    \
    TOKEN_DEF(TK_PAREN_L, '(')          \
    TOKEN_DEF(TK_PAREN_R, ')')          \
    TOKEN_DEF(TK_COMMA, ',')            \
    TOKEN_DEF(TK_COLON, ':')            \
    TOKEN_DEF(TK_BRACE_L, '[')          \
    TOKEN_DEF(TK_BRACE_R, ']')          \
    TOKEN_DEF(TK_DOLLA, '$')            \
    TOKEN_DEF(TK_CURL_L, '{')           \
    TOKEN_DEF(TK_CURL_R, '}')           \
    TOKEN_DEF(TK_QUOTE_SINGLE, '\'')    \
    TOKEN_DEF(TK_PLUS, '+')             \
    TOKEN_DEF(TK_STAR, '*')             \
    TOKEN_DEF(TK_SLASH, '/')            \
    TOKEN_DEF(TK_DASH, '-')             \
    TOKEN_DEF(TK_DOT, '.')              \
    TOKEN_DEF(TK_BANG, '!')             \
    TOKEN_DEF(TK_EQUALS, '=')           \
    TOKEN_DEF(TK_SPACE, ' ')            \
    TOKEN_DEF(TK_LT, '<')               \
    TOKEN_DEF(TK_GT, '>')               \
    TOKEN_DEF(TK_SEMI, ';')             \
    TOKEN_DEF(TK_NEWLINE, '\n')         \
    TOKEN_DEF(TK_RESERVED_TYPE, 0xF4)   \
    TOKEN_DEF(TK_LT_EQ, 0xF9u)          \
    TOKEN_DEF(TK_GT_EQ, 0xF8u)          \
    TOKEN_DEF(TK_STRICT_EQ, 0xF7u)      \
    TOKEN_DEF(TK_BANG_EQ, 0xF6u)        \
    TOKEN_DEF(TK_RESERVED, 0xF5u)       \
    TOKEN_DEF(TK_LITERAL_INT, 0xFEu)    \
    TOKEN_DEF(TK_LITERAL_FLOAT, 0xFDu)  \
    TOKEN_DEF(TK_IDENTIFIER, 0xFCu)     \
    TOKEN_DEF(TK_LITERAL_STRING, 0xFBu) \
    TOKEN_DEF(TK_LITERAL_DOUBLE, 0xFAu) \
    TOKEN_DEF(TK_UNKNOWN, 0xFFu)
// TOKEN_DEF(TK_LOL_JUDGE_DEEZ_NUTS, '\0')   \ // <------- for you dragon! (jk)

/// Enum representing token types
#define TOKEN_DEF(NAME, VALUE) NAME = VALUE,
enum GLOBAL_TOKENS : unsigned char {
    TOKEN_DEFs
};
#undef TOKEN_DEF
}  // namespace acc
