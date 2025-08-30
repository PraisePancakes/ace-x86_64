#pragma once
#include <iostream>
#include <vector>

#include "accast.hpp"
#include "acctoken.hpp"
#define DISCARD(f) (void)f
namespace acc {

class [[nodiscard]] acc_parser {
    std::vector<acc::token> m_input;
    std::vector<acc::StmtOwningVariant> m_ast;

    std::size_t m_end;

    [[nodiscard]] bool is_end() const noexcept {
        return m_end >= m_input.size();
    };

    [[nodiscard]] acc::token peek() const noexcept {
        return m_input[m_end];
    };

    [[nodiscard]] acc::token peek_prev() const noexcept {
        return m_input[m_end - 1];
    }

    [[nodiscard]] acc::token peek_next() const noexcept {
        return m_input[m_end + 1];
    }

    acc::token advance() noexcept {
        if (is_end()) {
            return peek();
        }
        return m_input[m_end++];
    };

    bool match_it(acc::ACC_ALL_TOKEN_ENUM type) noexcept {
        if (peek().type == type) {
            DISCARD(advance());
            return true;
        }
        return false;
    }

    auto match_any(auto&&... types)
        -> decltype((std::is_same_v<decltype(types),
                                    acc::ACC_ALL_TOKEN_ENUM> &&
                     ...),
                    std::true_type{}) {
        return (match_it(types) || ...);
    };

    void parse() const noexcept {

    };

   public:
    acc_parser(const acc_parser&) = delete;
    acc_parser& operator=(const acc_parser&) = delete;
    acc_parser(acc_parser&&) = default;
    acc_parser& operator=(acc_parser&&) = default;
    ~acc_parser() = default;
};
}  // namespace acc