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
            std::cout << "DIGIT FOUND :  " << v.value() << "\n";
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

    // ./testing -tc=*Monadic* -sc=*Number* --no-capture -s
    SUBCASE("Number") {
        {
            INFO("Finds number in sequence of tokens, and returns that number.");
            // success
            std::stringstream ss;
            ss << "12a";
            auto v = acc::int_()(ss);
            std::cout << "NUMBER FOUND :  " << v.value() << "\n";
            CHECK_EQ(v.value(), 12);
        }

        {
            INFO("Fails because next token in sequence is not a number.");
            // failure
            std::stringstream ss;
            ss << "a12b";
            auto v = acc::int_()(ss);
            INFO(v.error());
            CHECK_FALSE_MESSAGE(v.has_value(), "fails at a");
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

    // ./testing -tc=*Monadic* -sc=*Either* --no-capture -s
    SUBCASE("Either") {
        // either_1 (lhs)
        {
            std::stringstream ss;
            ss << "hello world";
            auto v = acc::either_1(acc::match_("hello")(ss), acc::match_("world")(ss));
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
            ss << "hello123";
            auto v = acc::either_2(acc::match_("hello")(ss), acc::match_("world")(ss));
            CHECK(v.has_value());
            CHECK(v.value() == "world");
        }
    }

    // ./testing -tc=*Monadic* -sc=*Ignore* --no-capture -s
    SUBCASE("Ignore") {
    }
}