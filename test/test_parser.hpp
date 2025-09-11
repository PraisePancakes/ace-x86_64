#pragma once
#include "../accx86_64/frontend/acclexer.hpp"
#include "../accx86_64/frontend/accparser.hpp"
#include "../accx86_64/frontend/statics/ro_accdelims.hpp"
#include "../accx86_64/frontend/statics/ro_accpairdelims.hpp"
#include "../accx86_64/frontend/statics/ro_acctypes.hpp"
#include "../accx86_64/utils/eval.hpp"

int TEST_PARSER() {
    acc::lexer lexe("(123 + (324 * 1)) < 1095",
                    acc::globals::ACC_DELIMS,
                    acc::globals::ACC_PAIR_DELIMS,
                    acc::globals::ACC_TYPE_SET);
    auto ts = lexe.lex();
    for (auto& w : ts) {
        w.print_token();
    }
    acc::acc_parser pr(ts);
    auto v = pr.parse();
    pr.print_ast();
    acc::interp::expr_eval eval;
    std::cout << eval.as<bool>(v[0]);
    return 0;
};