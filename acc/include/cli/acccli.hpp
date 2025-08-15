#pragma once
#include <cassert>
#include <cstdint>
#include <iostream>

#include "../acclog.hpp"
#include "prs_combinator.hpp"

/**
 *  BNF
 * ______
 *
 * <major> ::= <dash><dash><ident><minor>
 * <minor> ::= <dash><ident>
 * <ident> ::= <string>
 *
 */
/**
 * acc cli usage : [protocol]  --[major command]-[minor command]  [-[list of minor command flags], -[...]] [input file].[ace] -[output type]  [executable name]
 *
 * e.g.
 *
 *
 * two dash equals commands      one dash equals options                       output option : binary
 * (ace protocol) (dev flags)    (verbose lexer) (verbose ast)  (input file)  (output type flag (binary))     (executable name)
 * acc             --set-dev    [-verbose-lexer -verbose-ast ]  f.ace         -o                              fexec
 *
 * (ace protocol)   (help)
 * acc              --help-all
 * acc              --help-dev
 */
namespace acc {

class cli {
    enum class COMMANDS : std::uint8_t {
        ACE_PROTOCOL = 1 << 1,
        DEV_FLAGS = 1 << 2,
        HELP = 1 << 3,
        VERBOSE_LEXER = 1 << 4,
        VERBOSE_AST = 1 << 5
    };

   public:
    cli(std::stringstream& ss) {

    };
};
}  // namespace acc
