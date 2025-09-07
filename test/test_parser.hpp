#pragma once
#include "../acc/frontend/acclexer.hpp"
#include "../acc/frontend/accparser.hpp"
#include "../acc/utils/eval.hpp"

int TEST_PARSER() {
    acc::lexer lexe({acc::ACC_ALL_TOKEN_ENUM::TK_SPACE,
                     acc::ACC_ALL_TOKEN_ENUM::TK_PLUS,
                     acc::ACC_ALL_TOKEN_ENUM::TK_DASH,
                     acc::ACC_ALL_TOKEN_ENUM::TK_STAR,
                     acc::ACC_ALL_TOKEN_ENUM::TK_BANG,
                     acc::ACC_ALL_TOKEN_ENUM::TK_PAREN_L,
                     acc::ACC_ALL_TOKEN_ENUM::TK_PAREN_R},
                    "(123 + (324 * 3)) ");
    auto ts = lexe.lex();
    acc::acc_parser pr(ts);
    auto v = pr.parse();
    pr.print_ast();
    acc::interp::expr_eval eval;
    std::cout << eval.as<int>(v[0]);
    return 0;
};