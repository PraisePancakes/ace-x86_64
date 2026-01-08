#include "../accx86_64/frontend/acclexer.hpp"
#include "../accx86_64/frontend/accparser.hpp"
#include "../accx86_64/frontend/analysis/analyzer.hpp"
#include "../accx86_64/frontend/printers/ast_printer.hpp"
#include "../accx86_64/frontend/statics/tkxmacro.hpp"
#include "../accx86_64/utils/eval.hpp"
#include "doctest.hpp"
TEST_CASE("Type analysis"){
    // ./testing -tc=*Type* -sc=*declaration* --no-capture
    SUBCASE("Declaration"){
        acc::acc_lexer lxr(R"(
                bool x : mut = 1;
                bool y : mut = 'a'; //compile error
            )",
                           acc::globals::token_map);
auto ts = lxr.lex();
acc::acc_parser prs(ts);
auto v = prs.parse();
acc::output::ast_printer printer(std::cout);

printer.dump(v->get_items());
auto tc = acc::analyzer();
tc.try_analyze(v);
}

// ./testing -tc=*Parser* -sc=*type* --no-capture
SUBCASE("type") {
    acc::acc_lexer lxr(R"(
               char *y = 'c';
               type Foo {
                bool public x : mut = y;
                Foo()  {};
                int test() private {};
               };

               int x() {
                    Foo g = Foo();
               };

            )",
                       acc::globals::token_map);

    auto ts = lxr.lex();
    acc::acc_parser prs(ts);

    auto v = prs.parse();
    auto tc = acc::analyzer();
    tc.try_analyze(v);
    acc::output::ast_printer printer(std::cout);
    printer.dump(v->get_items());
}
}
;