#include <cstring>
#include <sstream>

#include "../accx86_64/fnl/pmonadic.hpp"
#include "doctest.hpp"
TEST_CASE("Monadic Parser Test") {
    // ./testing -tc=*Monadic* -sc=*Match* --no-capture
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
            std::cout << hello.error() << std::endl;
            CHECK_FALSE_MESSAGE(hello.has_value(), "should fail because hello is not in string");
        }
    }

    // ./testing -tc=*Monadic* -sc=*Ignore* --no-capture
    SUBCASE("Ignore") {
    }
}