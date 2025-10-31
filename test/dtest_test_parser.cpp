#include "../accx86_64/frontend/acclexer.hpp"
#include "../accx86_64/frontend/accparser.hpp"
#include "../accx86_64/frontend/statics/tkxmacro.hpp"
#include "../accx86_64/utils/eval.hpp"
#include "doctest.hpp"

TEST_CASE("Parser Analysis") {
    SUBCASE("Parse Declaration") {
        acc::lexer lxr(R"(
                int x : mut = 4;
            )",
                       acc::globals::token_map);
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
                       acc::globals::token_map);

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

#define COMPLEX_PARSE_TEST_WITH_ERR false
#if COMPLEX_PARSE_TEST_WITH_ERR
    SUBCASE("complex ast print ( with errors )") {
        acc::lexer lxr(R"(
               int x : mut = 4;
               x = 5;

               int y = 2;
               y = 1; // ERROR : assignment to const-qual

               {    
                    int y : mut = 6;
                    y = 8; 

                    {
                        int x = 5;
                        x = 8;  //ERROR : assignment to const-qual
                    };

                    {
                        int x : mut = 19;
                        x = 1;
                    };
               };


            )",
                       acc::globals::token_map);

        auto ts = lxr.lex();
        acc::acc_parser prs(ts);
        auto v = prs.parse();
        prs.print_ast();
    }
#endif
#if !COMPLEX_PARSE_TEST_WITH_ERR
    SUBCASE("complex ast print ( without errors )") {
        acc::lexer lxr(R"(
               int x : mut = 4;
               x = 5;

               int y = 2;

               {    
                    int y : mut = 6;
                    y = 8; 

                    {
                        int x = 5;
                    };

                    {
                        int x : mut = 19;
                        x = 1;
                    };
               };

            )",
                       acc::globals::token_map);

        auto ts = lxr.lex();
        acc::acc_parser prs(ts);
        auto v = prs.parse();
        prs.print_ast();
    }
#endif

    // ./testing -tc=*Parser* -sc=*functions*-definition-w-default* --no-capture
    SUBCASE("functions-definition-w-default") {
        acc::lexer lxr(R"(

            int f(int x : mut = 3, int y : mut = 2) {
                int z = x + 4;
                int h = x + y;
            };

            )",
                       acc::globals::token_map);

        auto ts = lxr.lex();
        acc::acc_parser prs(ts);
        auto v = prs.parse();
        auto* fstmt = std::get<acc::node::FuncStmt*>(v[0]);
        prs.print_ast();
        REQUIRE(fstmt->type.type == acc::GLOBAL_TOKENS::TK_RESERVED_TYPE);
        REQUIRE(fstmt->type.word == "int");
        REQUIRE([stmt = std::as_const(fstmt)]() -> bool {
            auto p1 = std::get<acc::node::DeclarationStmt*>(stmt->params[0]);
            bool has_const = p1->has_const(p1->cv_qual_flags);
            return !has_const;
        }());
    }
    // ./testing -tc=*Parser* -sc=*functions*-definition-wo-default* --no-capture
    SUBCASE("functions-definition-wo-default") {
        acc::lexer lxr(R"(
                int z = 4;
                int x = 2;
                int h(int x : mut, int y) {
                    int z = x + y;
                     
                };
            )",
                       acc::globals::token_map);

        auto ts = lxr.lex();
        acc::acc_parser prs(ts);
        auto v = prs.parse();
        prs.print_ast();
        if (v.size() > 0) {
            auto* fstmt = std::get<acc::node::FuncStmt*>(v[2]);
            REQUIRE(fstmt->type.type == acc::GLOBAL_TOKENS::TK_RESERVED_TYPE);
            REQUIRE(fstmt->type.word == "int");
            REQUIRE([stmt = std::as_const(fstmt)]() -> bool {
                auto p1 = std::get<acc::node::DeclarationStmt*>(stmt->params[0]);
                bool has_const = p1->has_const(p1->cv_qual_flags);
                return !has_const;
            }());
        }
    }
}