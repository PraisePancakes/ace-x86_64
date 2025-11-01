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

    enum class FLAGS : std::underlying_type_t<flag_size_t> {
        ACC = 0x1u,
        SET_DEV = 0x2u,
        VLEXER = 0x4u,
        VAST = 0x8u,
        HELP_DEV = 0x10u,
        HELP_ALL = 0x20u
    };

    flag_size_t m_build_flags{};

    void parse_input_file(std::stringstream& ss) {
        acc::many_(acc::ignore_(acc::match_(' ')))(ss);
        while (true) {
            acc::many_(acc::ignore_(acc::match_(' ')))(ss);
            if (auto v = acc::sequ_(acc::alnum_("invalid file name"),
                                    acc::match_(".ace", "Unknown file extension."))(ss)) {
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
            while (auto v = acc::any_(acc::transform_(acc::match_("-verbose-lexer"), []() { return FLAGS::VLEXER; }),
                                      acc::transform_(acc::match_("-verbose-ast"), []() { return FLAGS::VAST; }))(ss)) {
                acc::many_(acc::ignore_(acc::match_(' ')))(ss);
                m_build_flags |= (flag_size_t)v.value();
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
        auto v = acc::any_(acc::transform_(acc::match_("--set-dev"), []() {
                               return FLAGS::SET_DEV;
                           }),
                           acc::transform_(acc::match_("--help"), []() {
                               return FLAGS::HELP_ALL;
                           }),
                           acc::transform_(acc::match_("--help-dev"), []() {
                               return FLAGS::HELP_DEV;
                           }))(ss);
        if (v.has_value()) {
            m_build_flags |= (flag_size_t)v.value();
            if (v.value() == FLAGS::SET_DEV) {
                parse_dev_commands(ss);
            }
        }
    };

    void parse(std::stringstream& ss) {
        auto v = acc::transform_(acc::match_("acc"), []() { return FLAGS::ACC; })(ss);
        if (v.has_value()) {
            m_build_flags |= (flag_size_t)v.value();
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
        acc::logger::instance().send(logger::LEVEL::INFO, R"(
            *  BNF
            * ______
            * <protocol>   ::= acc <major>
            * <major>      ::= -((- flag) | f)[-<minor>]
            * <minor>      ::= (flag)[filters]
        )");
    };

    [[nodiscard]] bool constexpr is_set(FLAGS f) const noexcept {
        return ((m_build_flags & (flag_size_t)f) == (flag_size_t)f);
    };

   public:
    cli(std::stringstream&& ss) {
        parse(ss);
        if (is_set(FLAGS::ACC)) {
            if (is_set(FLAGS::HELP_ALL)) {
                print_usage_all();
            } else if (is_set(FLAGS::HELP_DEV)) {
                print_usage_devs();
            } else if (is_set(FLAGS::SET_DEV)) {
                if (is_set(FLAGS::VAST)) {
                    acc::logger::instance().send(logger::LEVEL::INFO, "[DEV FLAG] -verbose-ast has been set", std::cout);
                }
                if (is_set(FLAGS::VLEXER)) {
                    acc::logger::instance().send(logger::LEVEL::INFO, "[DEV FLAG] -verbose-lexer has been set", std::cout);
                }
            }
        }
    };
};
}  // namespace acc
