#include <cstring>

#include "../accx86_64/frontend/printers/token_printer.hpp"
#include "../accx86_64/frontend/statics/tkxmacro.hpp"
#include "doctest.hpp"
TEST_CASE("Token Analysis") {
    SUBCASE("Token to Stringized") {
        CHECK(std::strcmp(acc::token_printer::to_stringized(acc::GLOBAL_TOKENS::TK_PAREN_L), "'('") == 0);
        CHECK(std::strcmp(acc::token_printer::to_stringized(acc::GLOBAL_TOKENS::TK_PAREN_R), "')'") == 0);
        CHECK(std::strcmp(acc::token_printer::to_stringized(acc::GLOBAL_TOKENS::TK_COMMA), "','") == 0);
        CHECK(std::strcmp(acc::token_printer::to_stringized(acc::GLOBAL_TOKENS::TK_COLON), "':'") == 0);
        CHECK(std::strcmp(acc::token_printer::to_stringized(acc::GLOBAL_TOKENS::TK_BRACE_L), "'['") == 0);
        CHECK(std::strcmp(acc::token_printer::to_stringized(acc::GLOBAL_TOKENS::TK_BRACE_R), "']'") == 0);
        CHECK(std::strcmp(acc::token_printer::to_stringized(acc::GLOBAL_TOKENS::TK_DOLLA), "'$'") == 0);
        CHECK(std::strcmp(acc::token_printer::to_stringized(acc::GLOBAL_TOKENS::TK_CURL_L), "'{'") == 0);
        CHECK(std::strcmp(acc::token_printer::to_stringized(acc::GLOBAL_TOKENS::TK_CURL_R), "'}'") == 0);
        CHECK(std::strcmp(acc::token_printer::to_stringized(acc::GLOBAL_TOKENS::TK_PLUS), "'+'") == 0);
        CHECK(std::strcmp(acc::token_printer::to_stringized(acc::GLOBAL_TOKENS::TK_STAR), "'*'") == 0);
        CHECK(std::strcmp(acc::token_printer::to_stringized(acc::GLOBAL_TOKENS::TK_SLASH), "'/'") == 0);
        CHECK(std::strcmp(acc::token_printer::to_stringized(acc::GLOBAL_TOKENS::TK_DASH), "'-'") == 0);
        CHECK(std::strcmp(acc::token_printer::to_stringized(acc::GLOBAL_TOKENS::TK_DOT), "'.'") == 0);
        CHECK(std::strcmp(acc::token_printer::to_stringized(acc::GLOBAL_TOKENS::TK_BANG), "'!'") == 0);
        CHECK(std::strcmp(acc::token_printer::to_stringized(acc::GLOBAL_TOKENS::TK_EQUALS), "'='") == 0);
        CHECK(std::strcmp(acc::token_printer::to_stringized(acc::GLOBAL_TOKENS::TK_SPACE), "' '") == 0);
        CHECK(std::strcmp(acc::token_printer::to_stringized(acc::GLOBAL_TOKENS::TK_LT), "'<'") == 0);
        CHECK(std::strcmp(acc::token_printer::to_stringized(acc::GLOBAL_TOKENS::TK_GT), "'>'") == 0);
        CHECK(std::strcmp(acc::token_printer::to_stringized(acc::GLOBAL_TOKENS::TK_SEMI), "';'") == 0);
        CHECK(std::strcmp(acc::token_printer::to_stringized(acc::GLOBAL_TOKENS::TK_NEWLINE), "'\\n'") == 0);
    };

    SUBCASE("Token to String") {
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_RESERVED), "TK_RESERVED") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_RESERVED_TYPE), "TK_RESERVED_TYPE") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_QUOTE_DOUBLE), "TK_QUOTE_DOUBLE") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_PAREN_L), "TK_PAREN_L") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_PAREN_R), "TK_PAREN_R") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_COMMA), "TK_COMMA") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_COLON), "TK_COLON") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_BRACE_L), "TK_BRACE_L") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_BRACE_R), "TK_BRACE_R") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_DOLLA), "TK_DOLLA") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_CURL_L), "TK_CURL_L") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_CURL_R), "TK_CURL_R") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_QUOTE_SINGLE), "TK_QUOTE_SINGLE") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_PLUS), "TK_PLUS") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_STAR), "TK_STAR") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_SLASH), "TK_SLASH") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_DASH), "TK_DASH") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_DOT), "TK_DOT") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_BANG), "TK_BANG") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_EQUALS), "TK_EQUALS") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_SPACE), "TK_SPACE") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_LT), "TK_LT") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_GT), "TK_GT") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_SEMI), "TK_SEMI") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_NEWLINE), "TK_NEWLINE") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_BANG_EQ), "TK_BANG_EQ") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_STRICT_EQ), "TK_STRICT_EQ") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_GT_EQ), "TK_GT_EQ") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_LT_EQ), "TK_LT_EQ") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_LITERAL_DOUBLE), "TK_LITERAL_DOUBLE") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_LITERAL_STRING), "TK_LITERAL_STRING") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_IDENTIFIER), "TK_IDENTIFIER") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_LITERAL_FLOAT), "TK_LITERAL_FLOAT") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_LITERAL_INT), "TK_LITERAL_INT") == 0);
        CHECK(std::strcmp(acc::token_printer::to_string(acc::GLOBAL_TOKENS::TK_UNKNOWN), "TK_UNKNOWN") == 0);
    }

    SUBCASE("Token to Literal (type id)") {
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_RESERVED) == "x1ul");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_RESERVED_TYPE) == "x2ul");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_PAREN_L) == "(");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_PAREN_R) == ")");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_COMMA) == ",");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_COLON) == ":");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_BRACE_L) == "[");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_BRACE_R) == "]");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_DOLLA) == "$");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_CURL_L) == "{");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_CURL_R) == "}");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_PLUS) == "+");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_STAR) == "*");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_SLASH) == "/");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_DASH) == "-");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_DOT) == ".");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_BANG) == "!");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_EQUALS) == "=");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_SPACE) == " ");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_LT) == "<");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_GT) == ">");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_SEMI) == ";");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_BANG_EQ) == "xF6ul");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_STRICT_EQ) == "xF7ul");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_GT_EQ) == "xF8ul");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_LT_EQ) == "xF9ul");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_LITERAL_DOUBLE) == "xFAul");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_LITERAL_STRING) == "xFBul");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_IDENTIFIER) == "xFCul");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_LITERAL_FLOAT) == "xFDul");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_LITERAL_INT) == "xFEul");
        CHECK(acc::token_printer::to_literal(acc::GLOBAL_TOKENS::TK_UNKNOWN) == "xFFul");
    }
}