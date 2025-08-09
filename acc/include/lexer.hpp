#pragma once
#include <iostream>

namespace acc {
struct [[nodiscard]] lexeme {
    std::string word;
    std::pair<int, int> location;
    lexeme() = default;
    lexeme(const std::string& word) : word(word), location(0, 0) {};
    lexeme(const std::string& word, std::pair<int, int> l) : word(word), location(l) {};
    lexeme(const std::string& word, int x, int y) : word(word), location(std::make_pair(x, y)) {};
    lexeme(const lexeme& o) : word(o.word), location(o.location) {};
    lexeme(lexeme&& o) : word(std::move(o.word)), location(o.location) {};
    ~lexeme() = default;
};

class lexer {
    std::string m_output;

   public:
    lexer() {};
    lexer(std::string_view sv) {};
    ~lexer() {};
};
}  // namespace acc