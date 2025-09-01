#pragma once
#include "../frontend/acclexer.hpp"
#include "../frontend/accparser.hpp"

int TEST_PARSER() {
    acc::lexer lexe({acc::ACC_ALL_TOKEN_ENUM::TK_SPACE}, "123 \"456\"");
    auto ts = lexe.lex();
    for (auto t : ts) {
        t.print_token();
    }
    acc::acc_parser pr(ts);
    auto v = pr.parse();
    for (auto e : v) {
        pr.print_node(e);
    }
    return 0;
};