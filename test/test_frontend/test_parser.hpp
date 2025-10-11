#pragma once
#include "../../accx86_64/frontend/acclexer.hpp"
#include "../../accx86_64/frontend/accparser.hpp"
#include "../../accx86_64/frontend/statics/ro_accdelims.hpp"
#include "../../accx86_64/frontend/statics/ro_acckw.hpp"
#include "../../accx86_64/frontend/statics/ro_accpairdelims.hpp"
#include "../../accx86_64/utils/eval.hpp"

namespace acc::utest {
int TEST_PARSER() {
#if 1
    std::cout << "\n===== [ PARSER TEST ] =====\n";

    {
        std::cout << "\n{ ==EXPRESSIONS== }\n\n";
        acc::lexer lexe("(123 + (324 * 1)) < 1095;",
                        acc::globals::ACC_DELIMS,
                        acc::globals::ACC_PAIR_DELIMS,
                        acc::globals::ACC_KW_SET);
        auto ts = lexe.lex();
        acc::acc_parser pr(ts);
        auto v = pr.parse();
        pr.print_ast();
    }
#endif
#if 1
    {
        std::cout << "\n{ ==STATEMENTS== }\n\n";
        acc::lexer lexe(R"(
          
            {
                int x = 4;
                int y = 3;
                x = 5;
                y = 4;
                int z = 2
                
            };
          
            )",
                        acc::globals::ACC_DELIMS,
                        acc::globals::ACC_PAIR_DELIMS,
                        acc::globals::ACC_KW_SET);
        auto ts = lexe.lex();

        acc::acc_parser pr(ts);
        auto v = pr.parse();

        pr.print_ast();
    }
#endif
    return 0;
};

}  // namespace acc::utest
