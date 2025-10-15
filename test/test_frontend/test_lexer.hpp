#pragma once
#include "../../accx86_64/frontend/acclexer.hpp"
#include "../../accx86_64/frontend/accparser.hpp"
#include "../../accx86_64/frontend/statics/ro_accdelims.hpp"
#include "../../accx86_64/frontend/statics/ro_acckw.hpp"
#include "../../accx86_64/frontend/statics/ro_accpairdelims.hpp"
#include "../../accx86_64/frontend/statics/ro_acctypes.hpp"
#include "../../accx86_64/utils/eval.hpp"

namespace acc::utest {
int TEST_LEXER() {
    std::cout << "\n\n===== [ LEXER TEST ] =====\n\n";
    acc::lexer lexe("if(x == 4) {}",
                    acc::globals::ACC_DELIMS,
                    acc::globals::ACC_PAIR_DELIMS,
                    acc::globals::ACC_KW_SET,
                    acc::globals::ACC_KW_TYPE_SET);
    auto ts = lexe.lex();
    for (auto& w : ts) {
        w.print_token();
    }

    return 0;
};
}  // namespace acc::utest
