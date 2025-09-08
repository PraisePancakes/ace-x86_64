#pragma once
#include <iostream>
#include <stack>
#include <unordered_map>

#include "ansi.hpp"
namespace acc {

class logger {
   public:
    enum class LEVEL {
        INFO,
        WARNING,
        ERROR,
        FATAL
    };

   private:
    logger() = default;
    ~logger() = default;
    logger(const logger&) = delete;
    void operator=(const logger&) = delete;
    logger(logger&&) = delete;

   public:
    static logger& instance() {
        static logger log;
        return log;
    };

    template <typename Out>
    void send(LEVEL level, const std::string& message, Out& out) {
        auto get_color_stream = [&](LEVEL lvl, std::ostream& os) {
            switch (lvl) {
                case LEVEL::INFO:
                    return acc::ansi::foreground_green;
                case LEVEL::FATAL:
                    return ansi::foreground_dark_red;
                case LEVEL::WARNING:
                    return ansi::foreground_yellow;
                case LEVEL::ERROR:
                    return ansi::foreground_light_red;
                default:
                    return ansi::foreground_grey;
            };
        };
        out << get_color_stream(level, out) << message << acc::ansi::reset << std::endl;
    }

    void send(LEVEL level, const std::string& message) {
        send(level, message, std::cout);
    }
};

};  // namespace acc