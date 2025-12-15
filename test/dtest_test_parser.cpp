#include "../accx86_64/frontend/acclexer.hpp"
#include "../accx86_64/frontend/accparser.hpp"
#include "../accx86_64/frontend/printers/ast_printer.hpp"
#include "../accx86_64/frontend/statics/tkxmacro.hpp"
#include "../accx86_64/utils/eval.hpp"
#include "doctest.hpp"

TEST_CASE("Parser Analysis") {
    SUBCASE("Parse Declaration") {
        acc::acc_lexer lxr(R"(
                int x : mut = 4;
            )",
                           acc::globals::token_map);
        auto ts = lxr.lex();
        acc::acc_parser prs(ts);
        auto v = prs.parse();

        const auto* var = std::get<acc::node::DeclarationStmt*>(v->get_items()[0]);

        CHECK(var->name.word == "x");
        CHECK(var->type.word == "int");
        CHECK(!var->has_const(var->cv_qual_flags));
        CHECK(!var->has_volatile(var->cv_qual_flags));
        CHECK(acc::interp::expr_eval{}.as<int>(var->expr.value()) == 4);
    }

    SUBCASE("Parse Block") {
        acc::acc_lexer lxr(R"(
                int y = 3;
                {
                    int x : mut = 4;
                };
            )",
                           acc::globals::token_map);

        auto ts = lxr.lex();
        acc::acc_parser prs(ts);
        auto v = prs.parse();
        const auto* var_y = std::get<acc::node::DeclarationStmt*>(v->get_items()[0]);
        CHECK(var_y->name.word == "y");
        CHECK(var_y->type.word == "int");
        CHECK(var_y->has_const(var_y->cv_qual_flags));
        CHECK(!var_y->has_volatile(var_y->cv_qual_flags));
        CHECK(acc::interp::expr_eval{}.as<int>(var_y->expr.value()) == 3);

        const auto* block = std::get<acc::node::BlockStmt*>(v->get_items()[1]);
        const auto* blx = std::get<acc::node::DeclarationStmt*>(block->env->get_items()[0]);
        CHECK(blx->name.word == "x");
        CHECK(blx->type.word == "int");
        CHECK(!blx->has_const(blx->cv_qual_flags));
        CHECK(acc::interp::expr_eval{}.as<int>(blx->expr.value()) == 4);
    }

#define COMPLEX_PARSE_TEST_WITH_ERR true
    // ./testing -tc=*Parser* -sc=*complex* --no-capture
#if COMPLEX_PARSE_TEST_WITH_ERR
    SUBCASE("complex ast print ( with errors )") {
        acc::acc_lexer lxr(R"(
               int x : mut = 4;
               x := 5;

               int y = 2;
               y := 1; // ERROR : assignment to const-qual

               {    
                    int y : mut = 6;
                    y := 8; 

                    {
                        int x = 5;
                        x := 8;  //ERROR : assignment to const-qual
                    };

                    {
                        int x : mut = 19;
                        x := 1;
                    };
               };


            )",
                           acc::globals::token_map);

        auto ts = lxr.lex();
        acc::acc_parser prs(ts);
        auto v = prs.parse();
        acc::output::ast_printer printer(std::cout);

        printer.dump(v->get_items());
    }
#endif

    // ./testing -tc=*Parser* -sc=*complex* --no-capture
#if !COMPLEX_PARSE_TEST_WITH_ERR
    SUBCASE("complex ast print ( without errors )") {
        acc::acc_lexer lxr(R"(
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
        acc::output::ast_printer printer(std::cout);
        printer.dump(v->get_items());
    }
#endif

    // ./testing -tc=*Parser* -sc=*functions*-definition-w-default* --no-capture
    SUBCASE("functions-definition-w-default") {
        acc::acc_lexer lxr(R"(
            int f(int x : mut = 3, int y : mut = 2) {
                int z = x + 4;
                int h = x + y;
            };

            )",
                           acc::globals::token_map);

        auto ts = lxr.lex();
        acc::acc_parser prs(ts);
        auto v = prs.parse();
        auto* fstmt = std::get<acc::node::FuncStmt*>(v->get_items()[0]);
        acc::output::ast_printer printer(std::cout);
        printer.dump(v->get_items());
        REQUIRE(fstmt->type.type == acc::GLOBAL_TOKENS::TK_RESERVED_TYPE);
        REQUIRE(fstmt->type.word == "int");
        REQUIRE([stmt = std::as_const(fstmt)]() -> bool {
            auto p1 = stmt->params[0];
            bool has_const = p1->has_const(p1->cv_qual_flags);
            return !has_const;
        }());
    }
    // ./testing -tc=*Parser* -sc=*functions*-definition-wo-default* --no-capture
    SUBCASE("functions-definition-wo-default") {
        acc::acc_lexer lxr(R"(
                int z = 4;
                int x = 2;
                int h(int h : mut, int y) {
                    int z : mut = h + y;
                    z := 5;
                    return z;

                };

            )",
                           acc::globals::token_map);

        auto ts = lxr.lex();
        acc::acc_parser prs(ts);
        auto v = prs.parse();
        acc::output::ast_printer printer(std::cout);
        printer.dump(v->get_items());
        if (v->get_items().size() > 0) {
            auto* fstmt = std::get<acc::node::FuncStmt*>(v->get_items()[2]);
            REQUIRE(fstmt->type.type == acc::GLOBAL_TOKENS::TK_RESERVED_TYPE);
            REQUIRE(fstmt->type.word == "int");
            REQUIRE([stmt = std::as_const(fstmt)]() -> bool {
                auto p1 = stmt->params[0];
                bool has_const = p1->has_const(p1->cv_qual_flags);
                return !has_const;
            }());
            REQUIRE(fstmt->body->env->get_items().size() == 3);
        }
    }

    // ./testing -tc=*Parser* -sc=*functions*-call* --no-capture
    SUBCASE("functions-call") {
        acc::acc_lexer lxr(R"(
                bool f() {};

                int h(int h : mut, int y) {
                    int z = h;
                    bool g = f();
                };

            )",
                           acc::globals::token_map);

        auto ts = lxr.lex();
        acc::acc_parser prs(ts);
        auto v = prs.parse();
        acc::output::ast_printer printer(std::cout);
        printer.dump(v->get_items());
        if (v->get_items().size() > 0) {
            auto* fstmt = std::get<acc::node::FuncStmt*>(v->get_items()[0]);
            REQUIRE(fstmt->type.type == acc::GLOBAL_TOKENS::TK_RESERVED_TYPE);
            REQUIRE(fstmt->type.word == "bool");
            REQUIRE(fstmt->body->env->get_items().size() == 0);
        }
    }

    // ./testing -tc=*Parser* -sc=*type* --no-capture
    SUBCASE("type") {
        acc::acc_lexer lxr(R"(
               int y = 2;
               type Foo {
                int public x : mut = y;
                Foo(int x) {};
                int test() private {};
               };

               int x() {
                    Foo g = Foo(2);
               };

            )",
                           acc::globals::token_map);

        auto ts = lxr.lex();
        acc::acc_parser prs(ts);

        auto v = prs.parse();
        acc::output::ast_printer printer(std::cout);
        printer.dump(v->get_items());
    }
}