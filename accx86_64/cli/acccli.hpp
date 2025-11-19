#pragma once
#include <cassert>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <unordered_map>

#include "../fnl/pmonadic.hpp"
#include "../frontend/acclexer.hpp"
#include "../frontend/accparser.hpp"
#include "../frontend/statics/ro_tokenmap.hpp"
#include "../utils/acclog.hpp"
#include "monaparsers.hpp"

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
        cli_error(const std::string& s) : what(s) {};
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

    // std::unordered_map<OPTIONS, printer*> dispatcher;

    std::uint8_t m_build_options{0};
    std::pair<std::string, std::string> m_output_info;
    std::optional<std::string> m_dump_path;  // if set use the path else dump to stdout

    void parse_info_cmd(std::stringstream& ss) noexcept {
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

    void parse_translation_cmd(std::stringstream& ss) {
        acc::ignore_ws_()(ss);

        const auto input_parser = acc::transform_(acc::many_(acc::sequ_(acc::ignore_ws_(),
                                                                        acc::mona::file_parser(),
                                                                        acc::ignore_ws_())),
                                                  [](auto many) {
                                                      std::vector<std::string> input_files;
                                                      for (auto each : many.first) {
                                                          const std::string file = std::get<1>(each);
                                                          input_files.push_back(file);
                                                      }
                                                      return input_files;
                                                  });
        const auto output_parser = acc::transform_(acc::sequ_(acc::any_(acc::match_("-s"),
                                                                        acc::match_("-o")),
                                                              acc::ignore_ws_(),
                                                              acc::letters_()),
                                                   [](auto seq) {
                                                       using out_type = std::string;
                                                       using out_file = std::string;
                                                       return std::pair<out_type, out_file>{std::get<0>(seq),
                                                                                            std::get<2>(seq)};
                                                   });

        const auto translation_seq_parser = acc::sequ_(output_parser, input_parser)(ss);
        if (!translation_seq_parser) throw cli_error(translation_seq_parser.error());

        m_output_info = std::get<0>(translation_seq_parser.value());
        m_input_files = std::get<1>(translation_seq_parser.value());
    }

    void
    parse_dev_cmd(std::stringstream& ss) {
        acc::ignore_ws_()(ss);

        const auto set_dev = acc::either_1(acc::match_("-sd"),
                                           acc::match_("--set-dev"))(ss);
        if (!set_dev) return;

        const auto options_parser = acc::many_(acc::any_(acc::transform_(acc::match_("--dump-tree "),
                                                                         []() -> OPTIONS { return OPTIONS::DUMP_TREE; }),
                                                         acc::transform_(acc::match_("--dump-tokens "),
                                                                         []() -> OPTIONS { return OPTIONS::DUMP_TOKENS; }),
                                                         acc::transform_(acc::match_("--dump-asm "),
                                                                         []() -> OPTIONS { return OPTIONS::DUMP_ASM; })));
        const auto dev_seq_parser = acc::sequ_(acc::ignore_ws_(), options_parser, acc::ignore_ws_())(ss);

        if (!dev_seq_parser) throw cli_error(dev_seq_parser.error());

        const auto options_vec = std::get<1>(dev_seq_parser.value()).first;
        this->m_build_options |= ((options_vec.size() == 0) * (flag_size_t)-1);
        for (auto const o : options_vec) {
            this->m_build_options |= (flag_size_t)o;
        };
        const auto dump_parser = acc::mona::file_parser()(ss);
        if (dump_parser) {
            this->m_dump_path = dump_parser.value();
        };
    };

    void execute_cmd_events() {
        // for each path do ( 1 : dump sd info outstream , 2 : compile )

        for (auto path : this->m_input_files) {
            std::ifstream ifs;
            ifs.open(path, std::ios_base::in);

            if (ifs.is_open()) {
                std::stringstream ss;
                ss << ifs.rdbuf();
                auto lexer = acc::acc_lexer(ss.str(), acc::globals::token_map);
                const auto tokens = lexer.lex();
                auto parser = acc::acc_parser(std::move(tokens));
                const auto statements = parser.parse();
            } else {
                std::cout << "FAILED";  // handle error
            }
        };
    };

   public:
    cli(std::stringstream&& ss) {
        acc::ignore_ws_()(ss);
        auto found_entry = acc::match_("acc", "No valid entry found")(ss);
        if (!found_entry) throw cli_error(found_entry.error());
        parse_info_cmd(ss);

        try {
            parse_dev_cmd(ss);

            parse_translation_cmd(ss);
        } catch (cli_error& err) {
            acc::logger::instance().send(logger::LEVEL::FATAL, err.what);
            throw 69420;
        };

        execute_cmd_events();
    };
};
}  // namespace acc::iface
