#include "../accx86_64/frontend/acclexer.hpp"
#include "../accx86_64/frontend/accparser.hpp"
#include "../accx86_64/frontend/statics/ro_accdelims.hpp"
#include "../accx86_64/frontend/statics/ro_acckw.hpp"
#include "../accx86_64/frontend/statics/ro_accpairdelims.hpp"
#include "../accx86_64/frontend/statics/ro_acctypes.hpp"
#include "../accx86_64/frontend/statics/tkxmacro.hpp"
#include "../accx86_64/utils/eval.hpp"
#include "doctest.hpp"

TEST_CASE("Parser Analysis") {
    SUBCASE("Parse Declaration") {
        acc::lexer lxr(R"(
                int x : mut = 4;
            )",
                       acc::globals::ACC_DELIMS,
                       acc::globals::ACC_PAIR_DELIMS,
                       acc::globals::ACC_KW_SET,
                       acc::globals::ACC_KW_TYPE_SET);
        auto ts = lxr.lex();
        acc::acc_parser prs(ts);
        auto v = prs.parse();

        const auto* var = std::get<acc::node::DeclarationStmt*>(v[0]);

        CHECK(var->name.word == "x");
        CHECK(var->type.word == "int");
        CHECK(!var->has_const(var->cv_qual_flags));
        CHECK(!var->has_volatile(var->cv_qual_flags));
        CHECK(acc::interp::expr_eval{}.as<int>(var->expr.value()) == 4);
    }

    SUBCASE("Parse Block") {
    }
}