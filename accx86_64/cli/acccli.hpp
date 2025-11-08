#pragma once
#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <unordered_map>

#include "../fnl/pmonadic.hpp"
#include "../utils/acclog.hpp"

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

    enum class CLI_FLAGS : std::underlying_type_t<flag_size_t> {
        ACC = 0x1u,
        SET_DEV = 0x2u,
        DLEXER = 0x4u,
        DAST = 0x8u,
        HELP_ALL = 0x10u,
        VERSION = 0x20u,
        ERRORANEOUS = 0xffu

    };

    flag_size_t m_build_flags{};

    void parse_input_file(std::stringstream& ss) {

    };

    void parse_dev_commands(std::stringstream& ss) {

    };

    void parse_commands(std::stringstream& ss) {

    };

    void print_usage_all() {
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
            * [-h, --help] :                                 print usage/options for compilation.
            * [-sd<options, ...>, --set-dev<options, ...>] : set development debug options ( see below )
            * options [ dump_tree | dump_ast | duml_asm ]    [ file ]
            * 
            * [-v, --version] :                              display compiler version.
            * 
        )");
    };

    [[nodiscard]] bool constexpr is_set(CLI_FLAGS f) const noexcept {
        return ((m_build_flags & (flag_size_t)f) == (flag_size_t)f);
    };

    std::array<std::function<result<CLI_FLAGS>(std::stringstream&)>, 2> cmd_parsers{
        // ENTRY
        [](std::stringstream& ss) -> result<CLI_FLAGS> { return acc::transform_(acc::match_("acc", "entry not found."), []() {
                                                             return CLI_FLAGS::ACC;
                                                         })(ss); },
        // COMMANDS
        [](std::stringstream& ss) -> result<CLI_FLAGS> { return acc::any_(/*COMMAND HELP*/ acc::transform_(acc::either_1(acc::match_("-h"), acc::match_("--help")),
                                                                                                           []() { return CLI_FLAGS::HELP_ALL; }),
                                                                          /*COMMAND VERSION*/ acc::transform_(acc::either_1(acc::match_("-v"), acc::match_("--version")), []() { return CLI_FLAGS::VERSION; }))(ss); }};

   public:
    cli(std::stringstream&& ss) {
        for (auto parser : cmd_parsers) {
            acc::ignore_(acc::many_(acc::match_(' ')))(ss);
            auto v = parser(ss);
            if (v.has_value()) {
                m_build_flags |= (flag_size_t)v.value();
            } else {
                acc::logger::instance().send(acc::logger::LEVEL::FATAL, v.error());
            };
        };
    };
};
}  // namespace acc
