#include <cstring>
#include <sstream>

#include "../accx86_64/fnl/pmonadic.hpp"
#include "doctest.hpp"
TEST_CASE("Monadic Parser Test") {
    // ./testing -tc=*Monadic* -sc=*Digit* --no-capture -s
    SUBCASE("Digit") {
        {
            INFO("Finds first digit in sequence of tokens, and returns that FIRST digit.");
            // success
            std::stringstream ss;
            ss << "12a";
            auto v = acc::digit_()(ss);
            INFO("DIGIT FOUND : ", v.value());
            CHECK_EQ(v.value(), 1);
        }

        {
            INFO("Fails because next token in sequence is not a digit.");
            // failure
            std::stringstream ss;
            ss << "a12b";
            auto v = acc::digit_()(ss);
            INFO(v.error());
            CHECK_FALSE_MESSAGE(v.has_value(), "fails at a");
        }
    }

    // ./testing -tc=*Monadic* -sc=*Letter* --no-capture -s
    SUBCASE("Letter") {
        {
            INFO("Finds first letter in sequence of tokens, and returns that FIRST letter.");
            // success
            std::stringstream ss;
            ss << "a12";
            auto v = acc::letter_()(ss);
            INFO("LETTER FOUND : ", v.value());
            CHECK_EQ(v.value(), 'a');
        }

        {
            INFO("Fails because next token in sequence is not a letter.");
            // failure
            std::stringstream ss;
            ss << "12b";
            auto v = acc::letter_()(ss);
            INFO(v.error());
            CHECK_FALSE_MESSAGE(v.has_value(), "fails at 1");
        }
    }

    // ./testing -tc=*Monadic* -sc=*Letters* --no-capture -s
    SUBCASE("Letters") {
        {
            INFO("Finds sequence of letters in sequence of tokens, and returns that sequence.");
            // success
            std::stringstream ss;
            ss << "abcdefg12";
            auto v = acc::letters_()(ss);
            INFO("LETTERS FOUND : ", v.value());
            CHECK_EQ(v.value(), "abcdefg");
        }

        {
            INFO("Fails because next token in sequence is not a letter nor sequence of letters.");
            // failure
            std::stringstream ss;
            ss << "12b";
            auto v = acc::letters_()(ss);
            INFO(v.error());
            CHECK_FALSE_MESSAGE(v.has_value(), "fails at 1");
        }
    }

    // ./testing -tc=*Monadic* -sc=*Alnum* --no-capture -s
    SUBCASE("Alnum") {
        {
            INFO("Finds sequence of alnum in sequence of tokens, and returns that sequence.");
            // success
            std::stringstream ss;
            ss << "abcdefg12./.--=";
            auto v = acc::alnum_()(ss);
            INFO("ALNUMS FOUND : ", v.value());
            CHECK_EQ(v.value(), "abcdefg12");
        }

        {
            INFO("Fails because next token in sequence is not alnum.");
            // failure
            std::stringstream ss;
            ss << ".-=123x";
            auto v = acc::alnum_()(ss);
            INFO(v.error());
            CHECK_FALSE_MESSAGE(v.has_value(), "fails at .");
        }
    }

    // ./testing -tc=*Monadic* -sc=*Integer* --no-capture -s
    SUBCASE("Integer") {
        {
            INFO("Finds positive integer in sequence of tokens, and returns that positive integer.");
            // success
            std::stringstream ss;
            ss << "12a";
            auto v = acc::int_()(ss);
            INFO("INTEGER FOUND : ", v.value());
            CHECK_EQ(v.value(), 12);
        }
        {
            INFO("Finds negative integer in sequence of tokens, and returns that negative integer.");
            // success
            std::stringstream ss;
            ss << "-12a";
            auto v = acc::int_()(ss);
            INFO("INTEGER FOUND : ", v.value());
            CHECK_EQ(v.value(), -12);
        }

        {
            INFO("Fails because next token in sequence is not an integer.");
            // failure
            std::stringstream ss;
            ss << "a12b";
            auto v = acc::int_()(ss);
            INFO(v.error());
            CHECK_FALSE_MESSAGE(v.has_value(), "fails at a");
        }

        {
            INFO("Fails because next token in sequence is a dash however not followed by number.");
            // failure
            std::stringstream ss;
            ss << "-abc";
            auto v = acc::int_()(ss);
            INFO(v.error());
            CHECK_FALSE_MESSAGE(v.has_value(), "fails at -");
        }
    }

    // ./testing -tc=*Monadic* -sc=*Double* --no-capture -s
    SUBCASE("Double") {
        {
            INFO("Finds positive double in sequence of tokens, and returns that positive double.");
            // success
            std::stringstream ss;
            ss << "12.423a";
            auto v = acc::double_()(ss);
            INFO("DOUBLE FOUND : ", v.value());
            CHECK_EQ(v.value(), 12.423);
        }
        {
            INFO("Finds negative double in sequence of tokens, and returns that negative double.");
            // success
            std::stringstream ss;
            ss << "-12.1342a";
            auto v = acc::double_()(ss);
            INFO("DOUBLE FOUND : ", v.value());
            CHECK_EQ(v.value(), -12.1342);
        }

        {
            INFO("Finds double with period and no digits after.");
            // success
            std::stringstream ss;
            ss << "-12.a";
            auto v = acc::double_()(ss);
            INFO("DOUBLE FOUND : ", v.value());
            CHECK_EQ(v.value(), -12.0);
        }

        {
            INFO("Fails because next token in sequence is not an double.");
            // failure
            std::stringstream ss;
            ss << "a12.425b";
            auto v = acc::double_()(ss);
            INFO(v.error());
            CHECK_FALSE_MESSAGE(v.has_value(), "fails at a");
        }

        {
            INFO("Fails because next token in sequence is a dash however not followed by double.");
            // failure
            std::stringstream ss;
            ss << "-abc";
            auto v = acc::double_()(ss);
            INFO(v.error());
            CHECK_FALSE_MESSAGE(v.has_value(), "fails at -");
        }
    }

    // ./testing -tc=*Monadic* -sc=*Match* --no-capture -s
    SUBCASE("Match") {
        {
            // success
            std::stringstream ss;
            ss << "hello world";
            auto hello = acc::match_("hello")(ss);
            CHECK(hello.has_value());
            CHECK(hello.value() == "hello");
            auto space = acc::match_(" ")(ss);
            CHECK(space.has_value());
            CHECK(space.value() == " ");
            auto world = acc::match_("world")(ss);
            CHECK(world.has_value());
            CHECK(world.value() == "world");
        }
        {
            // failure
            std::stringstream ss;
            ss << "world";
            auto hello = acc::match_("hello")(ss);
            INFO(hello.error());
            CHECK_FALSE_MESSAGE(hello.has_value(), "should pass as a failure because hello is not in string");
        }
    }
    // ./testing -tc=*Monadic* -sc=*Number* --no-capture -s
    SUBCASE("Number") {
        {
            // success double
            std::stringstream ss;
            ss << "123.43";
            auto dble = acc::number_<double>()(ss);
            CHECK(dble.has_value());
            CHECK(dble.value() == 123.43);
        }

        {
            // success int
            std::stringstream ss;
            ss << "123.43";
            auto integer = acc::number_<int>()(ss);
            CHECK(integer.has_value());
            CHECK(integer.value() == 123);
        }

        {
            // failure for unsupported float
            std::stringstream ss;
            ss << "123.43";
            auto f = acc::number_<float>()(ss);
            CHECK_FALSE_MESSAGE(f.has_value(), f.error(), "FAILS AT THE MOMENT FOR UNSUPPORTED TYPE ( FLOAT )");
        }
    }

    // ./testing -tc=*Monadic* -sc=*Satisfies* --no-capture -s
    SUBCASE("Satisfies") {
        {
            // success
            std::stringstream ss;
            ss << "hellohellohello";
            auto has_hello = acc::satisfies_(acc::many_1(acc::match_("hello")))(ss);
            CHECK(has_hello.has_value());
        }

        {
            // failure
            std::stringstream ss;
            ss << "WORLDWORLDWORLD";
            auto has_hello = acc::satisfies_(acc::many_1(acc::match_("hello")))(ss);
            INFO(has_hello.error());
            CHECK_FALSE_MESSAGE(has_hello.has_value(), "should pass as a failure because hello is not in string");
        }
    }

    // ./testing -tc=*Monadic* -sc=*Either* --no-capture -s
    SUBCASE("Either") {
        // either_1 (lhs)
        {
            std::stringstream ss;
            ss << "hello world";
            auto v = acc::either_1(acc::match_("hello"), acc::match_("world"))(ss);
            CHECK(v.has_value());
            CHECK(v.value() == "hello");
        }

        // either_2 (rhs)
        {
            std::stringstream ss;
            ss << "world world";
            auto v = acc::either_2(acc::match_("hello")(ss), acc::match_("world")(ss));
            CHECK(v.has_value());
            CHECK(v.value() == "world");
        }
    }

    // ./testing -tc=*Monadic* -sc=*Both* --no-capture -s
    SUBCASE("Both") {
        {
            std::stringstream ss;
            ss << "hello-123";
            auto v = acc::both_(acc::match_("hello"), acc::int_())(ss);
            CHECK(v.has_value());
            CHECK(std::get<0>(v.value()) == "hello");
            CHECK(std::get<1>(v.value()) == -123);
        }
    }

    // ./testing -tc=*Monadic* -sc=*Many* --no-capture -s
    SUBCASE("Many") {
        {
            INFO("Calls the parser many times until failure. This unit returns either an empty { vector<T>, std::string } or populated { vector<T>, std::string }");
            std::stringstream ss;
            ss << "0123456789";
            auto v = acc::many_(acc::digit_())(ss);

            auto new_v = acc::transform_result_(v, [](auto v) {
                std::string ret;
                for (int i = 0; i < v.first.size(); i++) {
                    ret += std::to_string(v.first[i]);
                }
                return ret;
            });

            CHECK(v.has_value());
            CHECK(new_v.value() == "0123456789");
            for (std::size_t i = 0; i < 9; i++) {
                CHECK(v.value().first[i] == i);
            }
        }
    }

    // ./testing -tc=*Monadic* -sc=*Any* --no-capture -s
    SUBCASE("Any") {
        {
#include <array>
            std::stringstream ss;
            ss << "acc -v";
            std::array<std::function<acc::result<int>(std::stringstream&)>, 2> cmd_parsers{
                // ENTRY
                [](std::stringstream& ss) -> acc::result<int> { return acc::transform_(acc::match_("acc", "entry not found."), []() {
                                                                    return 1;
                                                                })(ss); },
                // COMMANDS
                [](std::stringstream& ss) -> acc::result<int> { return acc::any_(/*COMMAND HELP*/ acc::transform_(acc::either_1(acc::match_("-h"), acc::match_("--help")),
                                                                                                                  []() { return 2; }),
                                                                                 /*COMMAND VERSION*/ acc::transform_(acc::either_1(acc::match_("-v"), acc::match_("--version")), []() { return 3; }))(ss); }};
            for (auto& parser : cmd_parsers) {
                acc::many_(acc::ignore_(acc::match_(' ')))(ss);
                auto v = parser(ss);
                CHECK(v.has_value());
                if (v.has_value()) {
                    std::cout << v.value() << "\n";
                }
            };
        }
    }

    // ./testing -tc=*Monadic* -sc=*Transform* --no-capture -s
    SUBCASE("Transform") {
        {
            std::stringstream ss;
            ss << "hi mate";
            auto transformed = acc::transform_(acc::match_("hi mate"), [](auto v) -> std::string {
                std::string ret;
                for (auto it = v.rbegin(); it != v.rend(); ++it) {
                    ret += *it;
                }
                return ret;
            })(ss);
            CHECK(transformed.has_value());
            CHECK(transformed.value() == "etam ih");
        }
    }

    // ./testing -tc=*Monadic* -sc=*Sequence* --no-capture -s
    SUBCASE("Sequence") {
        {
            std::stringstream ss;
            ss << "hello-123";
            auto v = acc::sequ_(acc::match_("hello"), acc::match_('-'), acc::int_())(ss);
            CHECK(v.has_value());
            CHECK(std::get<0>(v.value()) == "hello");
            CHECK(std::get<1>(v.value()) == '-');
            CHECK(std::get<2>(v.value()) == 123);
        }
    }

    // ./testing -tc=*Monadic* -sc=*Ignore* --no-capture -s
    SUBCASE("Ignore") {
        std::stringstream ss;
        ss << "aaaaabbbcaaacc";
        auto v = acc::many_(acc::either_1(acc::ignore_(acc::match_('a')), acc::ignore_(acc::match_('b'))))(ss);
        auto new_v = acc::transform_result_(v, [](auto v) {
            std::string ret;
            for (auto c : v.first) {
                ret += c.first;
            };
            return ret;
        });
        CHECK(v.has_value());
        CHECK(new_v.value() == "aaaaabbb");
    }
}