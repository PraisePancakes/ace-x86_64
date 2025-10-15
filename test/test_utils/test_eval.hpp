#pragma once
#include "../../accx86_64/frontend/acclexer.hpp"
#include "../../accx86_64/frontend/accparser.hpp"
#include "../../accx86_64/frontend/statics/ro_accdelims.hpp"
#include "../../accx86_64/frontend/statics/ro_acckw.hpp"
#include "../../accx86_64/frontend/statics/ro_accpairdelims.hpp"
#include "../../accx86_64/utils/eval.hpp"

namespace acc::utest {
int TEST_EVAL() {
    std::cout << "\n\n===== [ EVAL TEST ] =====\n\n";
    std::string to_ev = "(123 * 0 + 1);";
    acc::lexer lexe(to_ev,
                    acc::globals::ACC_DELIMS,
                    acc::globals::ACC_PAIR_DELIMS,
                    acc::globals::ACC_KW_SET, acc::globals::ACC_KW_TYPE_SET);
    std::cout << acc::ansi::foreground_yellow << to_ev << acc::ansi::reset << std::endl;
    acc::interp::expr_eval ev;
    auto toks = lexe.lex();
    auto prsr = acc::acc_parser(toks);
    auto stmts = prsr.parse();

    std::cout << acc::ansi::foreground_green << std::boolalpha << ev.as<int>(stmts[0]) << acc::ansi::reset << std::endl;

    return 0;
};
}  // namespace acc::utest
