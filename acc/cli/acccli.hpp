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
    const std::unordered_map<std::string, std::uint8_t> m_flag_map{
        {"acc", 0x1u},
        {"--set-dev", 0x2u},
        {"-verbose-lexer", 0x4u},
        {"-verbose-ast", 0x8u},
        {"--help-all", 0x10u},
        {"--help-dev", 0x20u}

    };

    std::uint8_t m_build_flags{0};
    std::vector<std::string> m_input_files;

    void parse_input_file(std::stringstream& ss) {
        acc::many_(acc::ignore_(acc::match_(' ')))(ss);
        while (true) {
            acc::many_(acc::ignore_(acc::match_(' ')))(ss);
            if (auto v = acc::sequ_(acc::alnum_("invalid file name"), acc::match_(".ace", "Unknown file extension."))(ss)) {
                std::string path = std::apply([](auto&&... args) {
                    return (args + ...);
                },
                                              v.value());
                m_input_files.push_back(path);
            } else {
                if (!acc::eof_()(ss)) {
                    acc::logger::instance().send(acc::logger::LEVEL::FATAL, v.error(), std::cout);
                }
                break;
            }
        };

#if ACC__CLI__DEBUG
        if (m_input_files.size() > 0) {
            for (auto file : m_input_files) {
                std::cout << file << std::endl;
            }
        }
#endif
    };

    void parse_dev_commands(std::stringstream& ss) {
        acc::many_(acc::ignore_(acc::match_(' ')))(ss);
        if (acc::match_('[', "missing '[' ")(ss)) {
            acc::many_(acc::ignore_(acc::match_(' ')))(ss);
            while (auto v = acc::any_(acc::match_("-verbose-lexer"), acc::match_("-verbose-ast"))(ss)) {
                acc::many_(acc::ignore_(acc::match_(' ')))(ss);
                m_build_flags |= m_flag_map.at(v.value());
            }
            if (auto v = acc::match_(']', "missing dev command terminator ']'")(ss)) {
                return;
            } else {
                acc::logger::instance().send(acc::logger::LEVEL::FATAL, v.error(), std::cout);
                exit(EXIT_FAILURE);
            }
        }
    };

    void parse_commands(std::stringstream& ss) {
        acc::many_(acc::ignore_(acc::match_(' ')))(ss);
        auto v = acc::any_(acc::match_("--set-dev"),
                           acc::match_("--help-all"),
                           acc::match_("--help-dev"))(ss);
        if (v.has_value()) {
            m_build_flags |= m_flag_map.at(v.value());
            if (v.value() == "--set-dev") {
                parse_dev_commands(ss);
            }
        }
    };

    void parse(std::stringstream& ss) {
        auto v = acc::match_("acc")(ss);
        if (v.has_value()) {
            m_build_flags |= m_flag_map.at(v.value());
            parse_commands(ss);
            parse_input_file(ss);
        }
    };

    void print_usage_devs() {
        acc::logger::instance().send(logger::LEVEL::INFO,
                                     R"(FLAGS 
            --set-dev [-verbose-lexer , -verbose-ast]
                -verbose-lexer : prints the lexed result of each input file.
                -verbose-ast : prints the abstract-syntax-tree of each input file.
                                                                )",
                                     std::cout);
    }

    void print_usage_all() {
        print_usage_devs();
        acc::logger::instance().send(logger::LEVEL::INFO, "e.g.            acc                                                     foo.ace         -o                              fexec", std::cout);
        acc::logger::instance().send(logger::LEVEL::INFO, "         (ace protocol)  <optional flags> [optional flag options]    (input file)  (output type flag (binary))     (executable name)", std::cout);
    };

    [[nodiscard]] bool constexpr is_set(const std::string& f) const noexcept {
        return ((m_build_flags & m_flag_map.at(f)) == m_flag_map.at(f));
    };

   public:
    cli(std::stringstream&& ss) {
        parse(ss);
        if (is_set("acc")) {
            if (is_set("--help-all")) {
                print_usage_all();
            } else if (is_set("--help-dev")) {
                print_usage_devs();
            } else if (is_set("--set-dev")) {
                if (is_set("-verbose-ast")) {
                    acc::logger::instance().send(logger::LEVEL::INFO, "[DEV FLAG] -verbose-ast has been set", std::cout);
                }
                if (is_set("-verbose-lexer")) {
                    acc::logger::instance().send(logger::LEVEL::INFO, "[DEV FLAG] -verbose-lexer has been set", std::cout);
                }
            }
        }
    };
};
}  // namespace acc
