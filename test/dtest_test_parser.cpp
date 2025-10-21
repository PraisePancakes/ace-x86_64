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
        acc::lexer lxr(R"(
                int y = 3;
                {
                    int x : mut = 4;
                };
            )",
                       acc::globals::ACC_DELIMS,
                       acc::globals::ACC_PAIR_DELIMS,
                       acc::globals::ACC_KW_SET,
                       acc::globals::ACC_KW_TYPE_SET);

        auto ts = lxr.lex();
        acc::acc_parser prs(ts);
        auto v = prs.parse();
        const auto* var_y = std::get<acc::node::DeclarationStmt*>(v[0]);
        CHECK(var_y->name.word == "y");
        CHECK(var_y->type.word == "int");
        CHECK(var_y->has_const(var_y->cv_qual_flags));
        CHECK(!var_y->has_volatile(var_y->cv_qual_flags));
        CHECK(acc::interp::expr_eval{}.as<int>(var_y->expr.value()) == 3);

        const auto* block = std::get<acc::node::BlockStmt*>(v[1]);
        const auto* blx = std::get<acc::node::DeclarationStmt*>(block->stmts[0]);
        CHECK(blx->name.word == "x");
        CHECK(blx->type.word == "int");
        CHECK(!blx->has_const(blx->cv_qual_flags));
        CHECK(acc::interp::expr_eval{}.as<int>(blx->expr.value()) == 4);
    }
}