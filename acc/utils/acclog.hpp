#pragma once
#include <iostream>
#include <stack>
#include <unordered_map>
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
    std::unordered_map<LEVEL, std::string> m_log_tag{
        {LEVEL::INFO, "\x1b[1;32m[INFO]\x1b[0m "},
        {LEVEL::WARNING, "\x1b[1;33m[WARNING]\x1b[0m "},
        {LEVEL::ERROR, "\x1b[1;31m[ERROR]\x1b[0m "},
        {LEVEL::FATAL, "\x1b[0;31m[FATAL]\x1b[0m "}};

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
        out << m_log_tag[level] << message << std::endl;
    }

    void send(LEVEL level, const std::string& message) {
        std::cout << m_log_tag[level] << message << std::endl;
    }
};

};  // namespace acc