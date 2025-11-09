#pragma once
#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <unordered_map>

#include "../fnl/pmonadic.hpp"
#include "../utils/acclog.hpp"

#define NOOP__CALLABLE []() {}
#define ACC__CLI__DEBUG true
/**
 *  BNF
 * ______
 * <protocol>   ::= acc <major>
 * <major>      ::= -((- flag) | f)[-<minor>]
 * <minor>      ::= (flag)[filters]
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
 * acc              --help
 * acc              --help-dev
 */
namespace acc {

class cli {
    using flag_size_t = std::byte;
    std::vector<std::string> m_input_files;
    static void dump_usage() {
        acc::logger::instance().send(logger::LEVEL::INFO, R"(
            * USAGE
            * _____
            * 
            * acc.exe [COMMANDS] file...
            * 
            *  BNF
            * ______
            * <protocol>   ::= acc <major>
            * <major>      ::= -((- flag) | f)[-<minor>]
            * <minor>      ::= (flag)[filters]
            * 
            * 
            * COMMANDS
            * ________
            * 
            * [-h, --help]                                 : Print usage/options for compilation.
            * [-sd<options, ...>, --set-dev<options, ...>] : Set development debug options ( see below )
            * options [ dump_tree | dump_ast | duml_asm ]    [ file ]
            * 
            * [-v, --version]                              : Display compiler version.
            * [-o, --output]<file>                         : Place the output into <file>
            * 
        )");
    }

    static void dump_version() {};

    std::array<std::function<result<std::function<void()>>(std::stringstream&)>, 2> cmd_parsers{
        // ENTRY
        [](std::stringstream& ss)
            -> result<std::function<void()>> { return acc::transform_(acc::match_("acc", "entry not found."), []() -> std::function<void()> {
                                                   return NOOP__CALLABLE;
                                               })(ss); },
        // COMMANDS
        [this](std::stringstream& ss)
            -> result<std::function<void()>> { return acc::any_("unkown command, enter \"acc --help\" or \"acc -h\" for usage", /*COMMAND HELP*/ acc::transform_(acc::either_1(acc::match_("-h"), acc::match_("--help")), []() -> std::function<void()> { return acc::cli::dump_usage; }),
                                                                /*COMMAND VERSION*/ acc::transform_(acc::either_1(acc::match_("-v"), acc::match_("--version")), []() -> std::function<void()> { return acc::cli::dump_version; }))(ss); }};

   public:
    cli(std::stringstream&& ss) {
        try {
            for (auto parser : cmd_parsers) {
                acc::ignore_(acc::many_(acc::match_(' ')))(ss);
                auto v = parser(ss);
                if (v.has_value()) {
                    v.value()();
                } else {
                    acc::logger::instance().send(acc::logger::LEVEL::FATAL, v.error());
                    throw 42;
                };
            };
        } catch (...) {
            throw 1;
        };
    }
};
}  // namespace acc
