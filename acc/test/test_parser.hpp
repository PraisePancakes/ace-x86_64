#pragma once
#include "../frontend/acclexer.hpp"
#include "../frontend/accparser.hpp"

int TEST_PARSER() {
    acc::lexer lexe({acc::ACC_ALL_TOKEN_ENUM::TK_SPACE,
                     acc::ACC_ALL_TOKEN_ENUM::TK_PLUS,
                     acc::ACC_ALL_TOKEN_ENUM::TK_DASH,
                     acc::ACC_ALL_TOKEN_ENUM::TK_STAR},
                    "123 + 324 * 3");
    auto ts = lexe.lex();
    acc::acc_parser pr(ts);
    auto v = pr.parse();
    for (auto e : v) {
        pr.print_node(e);
    }
    return 0;
};