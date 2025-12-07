#include "../accx86_64/frontend/acclexer.hpp"
#include "../accx86_64/frontend/accparser.hpp"
#include "../accx86_64/frontend/analysis/analyzer.hpp"
#include "../accx86_64/frontend/printers/ast_printer.hpp"
#include "../accx86_64/frontend/statics/tkxmacro.hpp"
#include "../accx86_64/utils/eval.hpp"
#include "doctest.hpp"
// ./testing -tc=*Type* -sc=*declaration* --no-capture
TEST_CASE("Type analysis"){
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
auto tc = acc::analyzer(v);
tc.try_analyze();
}
}
;