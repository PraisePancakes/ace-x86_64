#include "../../accx86_64/frontend/statics/ro_accdelims.hpp"
#include "../../accx86_64/frontend/statics/ro_acckw.hpp"
#include "../../accx86_64/frontend/statics/ro_accpairdelims.hpp"
#include "../../accx86_64/frontend/statics/ro_acctypes.hpp"
#include "../../accx86_64/frontend/statics/tkxmacro.hpp"
#include "../accx86_64/frontend/acclexer.hpp"
#include "doctest.hpp"

TEST_CASE("Lexical Division") {
    acc::lexer lexe(R"(
        if 
        else 
        while 
        for 
        int 
        short 
        double 
        float 
        char 
        bool 
        long 
        (
        ) 
        {
        } 
        [
        ] 
        : 
        ; 
        ident 
        , 
        + 
        - 
        / 
        < 
        <= 
        > 
        >=

         
        )",
                    acc::globals::ACC_DELIMS,
                    acc::globals::ACC_PAIR_DELIMS,
                    acc::globals::ACC_KW_SET,
                    acc::globals::ACC_KW_TYPE_SET);
    auto toks = lexe.lex();

    REQUIRE(toks.size() == 8);
    CHECK(toks[0].type == acc::GLOBAL_TOKENS::TK_RESERVED);
    CHECK(toks[1].type == acc::GLOBAL_TOKENS::TK_RESERVED);
    CHECK(toks[2].type == acc::GLOBAL_TOKENS::TK_RESERVED);
    CHECK(toks[3].type == acc::GLOBAL_TOKENS::TK_RESERVED);
    CHECK(toks[4].type == acc::GLOBAL_TOKENS::TK_RESERVED_TYPE);
    CHECK(toks[5].type == acc::GLOBAL_TOKENS::TK_RESERVED_TYPE);
    CHECK(toks[6].type == acc::GLOBAL_TOKENS::TK_RESERVED_TYPE);
    CHECK(toks[7].type == acc::GLOBAL_TOKENS::TK_RESERVED_TYPE);
    CHECK(toks[8].type == acc::GLOBAL_TOKENS::TK_RESERVED_TYPE);
    CHECK(toks[9].type == acc::GLOBAL_TOKENS::TK_RESERVED_TYPE);
    CHECK(toks[10].type == acc::GLOBAL_TOKENS::TK_RESERVED_TYPE);
    CHECK(toks[11].type == acc::GLOBAL_TOKENS::TK_PAREN_L);
    CHECK(toks[12].type == acc::GLOBAL_TOKENS::TK_PAREN_R);
    CHECK(toks[13].type == acc::GLOBAL_TOKENS::TK_CURL_L);
    CHECK(toks[14].type == acc::GLOBAL_TOKENS::TK_CURL_R);
    CHECK(toks[15].type == acc::GLOBAL_TOKENS::TK_BRACE_L);
    CHECK(toks[16].type == acc::GLOBAL_TOKENS::TK_BRACE_R);
}