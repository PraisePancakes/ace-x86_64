#pragma once
#include "../accx86_64/frontend/acclexer.hpp"
#include "../accx86_64/frontend/accparser.hpp"
#include "../accx86_64/utils/eval.hpp"

int TEST_PARSER() {
    acc::lexer lexe({
                        acc::ACC_ALL_TOKEN_ENUM::TK_SPACE,
                        acc::ACC_ALL_TOKEN_ENUM::TK_PLUS,
                        acc::ACC_ALL_TOKEN_ENUM::TK_DASH,
                        acc::ACC_ALL_TOKEN_ENUM::TK_STAR,
                        acc::ACC_ALL_TOKEN_ENUM::TK_BANG,
                        acc::ACC_ALL_TOKEN_ENUM::TK_PAREN_L,
                        acc::ACC_ALL_TOKEN_ENUM::TK_PAREN_R,
                        acc::ACC_ALL_TOKEN_ENUM::TK_BANG,
                        acc::ACC_ALL_TOKEN_ENUM::TK_LT,
                        acc::ACC_ALL_TOKEN_ENUM::TK_GT,
                        acc::ACC_ALL_TOKEN_ENUM::TK_EQUALS
                    },
                    "(123 + (324 * 3)) ");
    auto ts = lexe.lex();
    acc::acc_parser pr(ts);
    auto v = pr.parse();
    pr.print_ast();
    acc::interp::expr_eval eval;
    std::cout << eval.as<int>(v[0]);
    return 0;
};