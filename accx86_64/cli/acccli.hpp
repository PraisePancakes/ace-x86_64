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
namespace acc::iface {
static void dump_usage() {
    acc::logger::instance().send(logger::LEVEL::INFO, R"(
* USAGE
* _____
* 
* acc.exe [META COMMANDS][TRANSLATION COMMANDS] input_files...
* 
*  BNF
* ______
* <protocol>   ::= acc <major>
* <major>      ::= -((- flag) | f)[-<minor>]
* <minor>      ::= (flag)[filters]
* 
* 
* META COMMANDS
* _____________
* 
* [-h || --help]                                                      : Print usage/options for compilation.
* [-sd<[options, ...], [file]> || --set-dev<[options, ...], [file]>]  : Set development debug options ( see below )
*       options [ --dump-tree | --dump-tokens | --dump-asm ] 
* 
* [-v || --version]                                                   : Display compiler version.
*
* TRANSLATION COMMANDS
* ____________________
*
* [-o || --output(exec)]<file>                                        : Place the translated output into <file> (executable)
* [-s || --asm]<file>                                                 : Place translated assembly into <file>
* 
        )");
}

static void dump_version() {};
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

    bool is_set(OPTIONS o) {
        return ((this->m_build_options & (flag_size_t)o) == (flag_size_t)o);
    };

    std::uint8_t m_build_options{0};
    std::optional<std::string> m_dump_path;  // if set use the path else dump to stdout

    void parse_info(std::stringstream& ss) {
        acc::ignore_ws_()(ss);
        auto help_or_version = acc::any_(acc::transform_(acc::either_1(acc::match_("-h"),
                                                                       acc::match_("--help")),
                                                         []() -> std::function<void()> { return acc::iface::dump_usage; }),
                                         acc::transform_(
                                             acc::either_1(acc::match_("-v"),
                                                           acc::match_("--version")),
                                             []() -> std::function<void()> { return acc::iface::dump_version; }))(ss);
        if (help_or_version.has_value()) {
            help_or_version.value()();
        };
    };

    void parse_translation(std::stringstream& ss) {
        acc::ignore_ws_()(ss);
    }

    void parse_dev(std::stringstream& ss) {
        acc::ignore_ws_()(ss);
        const auto sd_or_set_dev = acc::either_1(acc::match_("-sd"),
                                                 acc::match_("--set-dev"));
        const auto options_parser = acc::many_(acc::any_(acc::transform_(acc::match_("--dump-tree "),
                                                                         []() -> OPTIONS { return OPTIONS::DUMP_TREE; }),
                                                         acc::transform_(acc::match_("--dump-tokens "),
                                                                         []() -> OPTIONS { return OPTIONS::DUMP_TOKENS; }),
                                                         acc::transform_(acc::match_("--dump-asm "),
                                                                         []() -> OPTIONS { return OPTIONS::DUMP_ASM; })));

        const auto file_parser = acc::sequ_(acc::letters_(), acc::match_('.'), acc::letters_());

        const auto dev_seq_parser = acc::sequ_(sd_or_set_dev, acc::ignore_ws_(), options_parser, acc::ignore_ws_(), file_parser)(ss);

        if (dev_seq_parser) {
            const auto options_vec = std::get<2>(dev_seq_parser.value()).first;
            this->m_build_options |= (flag_size_t)-1;

            for (auto const o : options_vec) {
                this->m_build_options ^= (((flag_size_t)-1 & ((flag_size_t)o)));
            };

            this->m_build_options = ((this->m_build_options == (flag_size_t)-1) * (flag_size_t)-1 | (~this->m_build_options));

            this->m_dump_path = std::apply([](auto&&... tuple_args) {
                return (tuple_args + ...);
            },
                                           std::get<4>(dev_seq_parser.value()));
        }
    };

    void parse_acc_flags(std::stringstream& ss) {
        acc::ignore_ws_()(ss);
        auto found_entry = acc::match_("acc", "No valid entry found")(ss);

        if (found_entry) {
            parse_info(ss);
            parse_dev(ss);
            parse_translation(ss);
        } else {
            throw cli_error(found_entry.error());
        }
    };

   public:
    cli(std::stringstream&& ss) {
        try {
            parse_acc_flags(ss);
            std::cout << std::boolalpha << is_set(OPTIONS::DUMP_TREE) << "\n";
            std::cout << std::boolalpha << is_set(OPTIONS::DUMP_TOKENS) << "\n";
            std::cout << std::boolalpha << is_set(OPTIONS::DUMP_ASM) << "\n";

        } catch (cli_error& err) {
            acc::logger::instance().send(logger::LEVEL::FATAL, err.what);
            throw 69420;
        };
    };
};
}  // namespace acc::iface
