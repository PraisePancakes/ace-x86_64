#pragma once
#include "../../accx86_64/frontend/acclexer.hpp"
#include "../../accx86_64/frontend/accparser.hpp"
#include "../../accx86_64/frontend/statics/ro_accdelims.hpp"
#include "../../accx86_64/frontend/statics/ro_accpairdelims.hpp"
#include "../../accx86_64/frontend/statics/ro_acctypes.hpp"
#include "../../accx86_64/utils/eval.hpp"

namespace acc::utest {
int TEST_PARSER() {
    std::cout << "\n\n===== [ PARSER TEST ] =====\n\n";

    {
        std::cout << "\n{ ==EXPRESSIONS== }\n";
        acc::lexer lexe("(123 + (324 * 1)) < 1095;",
                        acc::globals::ACC_DELIMS,
                        acc::globals::ACC_PAIR_DELIMS,
                        acc::globals::ACC_TYPE_SET);
        auto ts = lexe.lex();
        acc::acc_parser pr(ts);
        auto v = pr.parse();
        pr.print_ast();
    }

    {
        std::cout << "\n{ ==DECLARATIONS== }\n";
        acc::lexer lexe(R"(
            int x = 3;
            x = 4;
            x = 5;
            x = 7;
            
            )",
                        acc::globals::ACC_DELIMS,
                        acc::globals::ACC_PAIR_DELIMS,
                        acc::globals::ACC_TYPE_SET);
        auto ts = lexe.lex();
        acc::acc_parser pr(ts);
        auto v = pr.parse();
        pr.print_ast();
    }

    return 0;
};
}  // namespace acc::utest
