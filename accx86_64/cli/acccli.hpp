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
    struct cli_error : std::exception {
        std::string what;
        cli_error(std::string& s) : what(s) {};
    };

    using flag_size_t = std::uint8_t;
    std::vector<std::string> m_input_files;
    enum class OPTIONS : flag_size_t {
        DUMP_TREE = 1 << 0,
        DUMP_TOKENS = 1 << 1,
        DUMP_ASM = 1 << 2
    };
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
* [-h || --help]                                                      : Print usage/options for compilation.
* [-sd<[options, ...], [file]> || --set-dev<[options, ...], [file]>]  : Set development debug options ( see below )
*       options [ --dump-tree | --dump-tokens | --dump-asm ] 
* 
* [-v || --version]                                                   : Display compiler version.
* [-o || --output]<file>                                              : Place the output into <file>
* 
        )");
    }

    static void dump_version() {};

    bool is_set(OPTIONS o) {
        return ((this->m_build_options & (flag_size_t)o) == (flag_size_t)o);
    };

    std::uint8_t m_build_options{0};
    std::string m_dump_path;

    void parse_info(std::stringstream& ss) {
        acc::many_(acc::ignore_(acc::match_(' ')))(ss);
        auto help_or_version = acc::any_(acc::transform_(acc::either_1(acc::match_("-h"),
                                                                       acc::match_("--help")),
                                                         []() -> std::function<void()> { return acc::cli::dump_usage; }),
                                         acc::transform_(
                                             acc::either_1(acc::match_("-v"),
                                                           acc::match_("--version")),
                                             []() -> std::function<void()> { return acc::cli::dump_version; }))(ss);
        if (help_or_version.has_value()) {
            help_or_version.value()();
        };
    };

    void parse_dev(std::stringstream& ss) {
        acc::many_(acc::ignore_(acc::match_(' ')))(ss);

        auto sd_or_set_dev = acc::either_1(acc::match_("-sd"),
                                           acc::match_("--set-dev"))(ss);
        if (sd_or_set_dev) {
            acc::many_(acc::ignore_(acc::match_(' ')))(ss);
            auto dev_seq = acc::many_(acc::any_(
                acc::transform_(acc::match_("--dump-tree "),
                                []() -> OPTIONS { return OPTIONS::DUMP_TREE; }),
                acc::transform_(acc::match_("--dump-tokens "),
                                []() -> OPTIONS { return OPTIONS::DUMP_TOKENS; }),
                acc::transform_(acc::match_("--dump-asm "),
                                []() -> OPTIONS { return OPTIONS::DUMP_ASM; })))(ss);

            if (dev_seq) {
                auto options_vec = dev_seq.value().first;
                for (auto const& o : options_vec) {
                    this->m_build_options |= (flag_size_t)o;
                }
            }
        }
    };

    void parse_acc_flags(std::stringstream& ss) {
        acc::many_(acc::ignore_(acc::match_(' ')))(ss);
        auto found_entry = acc::match_("acc", "No valid entry found")(ss);

        if (found_entry) {
            parse_info(ss);
            parse_dev(ss);
        } else {
            throw cli_error(found_entry.error());
        }
    };

   public:
    cli(std::stringstream&& ss) {
        try {
            parse_acc_flags(ss);
        } catch (cli_error& err) {
            acc::logger::instance().send(logger::LEVEL::FATAL, err.what);
            throw 69420;
        };
    };
};
}  // namespace acc
