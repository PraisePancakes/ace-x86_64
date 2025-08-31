#pragma once
#include <iostream>
#include <vector>

#include "../utils/visitor.hpp"
#include "accast.hpp"
#include "acctoken.hpp"
#define DISCARD(f) (void)f
namespace acc {

class [[nodiscard]] acc_parser {
    std::vector<acc::token> m_input;
    std::size_t m_end{0};
    std::vector<acc::ExprVariant> exprs;  // block will have this

    [[nodiscard]] bool is_end() const noexcept {
        return peek().type == acc::ACC_ALL_TOKEN_ENUM::TK_EOF;
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
    }

    bool check_it(acc::ACC_ALL_TOKEN_ENUM type) noexcept {
        if (is_end()) {
            return false;
        }
        return peek().type == type;
    }
    bool match_it(acc::ACC_ALL_TOKEN_ENUM type) noexcept {
        if (check_it(type)) {
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
    }

    acc::ExprVariant parse_primary() {
        if (match_it(TK_LITERAL_INT)) {
            return new acc::node::LiteralExpr{.value = peek_prev().value,
                                              .embedded = peek_prev()};
        }

        if (match_it(TK_LITERAL_STRING)) {
            return new acc::node::LiteralExpr{.value = peek_prev().value,
                                              .embedded = peek_prev()};
        }

        if (match_it(TK_LITERAL_DOUBLE)) {
            return new acc::node::LiteralExpr{.value = peek_prev().value,
                                              .embedded = peek_prev()};
        }

        if (match_it(TK_LITERAL_FLOAT)) {
            return new acc::node::LiteralExpr{.value = peek_prev().value,
                                              .embedded = peek_prev()};
        }

        throw std::runtime_error("PARSERROR");
    }

   public:
    acc_parser(const std::vector<acc::token>& toks) : m_input(toks) {

                                                      };

    void print_node(acc::ExprVariant expr) {
        std::visit(internal::visitor{[&](acc::node::BinaryExpr* bxpr) {
                                         std::cout << " [ BINARY EXPR ] " << std::endl;
                                         print_node(bxpr->lhs);
                                         std::cout << bxpr->op.word << std::endl;
                                         print_node(bxpr->rhs);
                                     },
                                     [](acc::node::LiteralExpr* lxpr) {
                                         std::cout << " [ LITERAL EXPR ] " << std::endl;
                                         lxpr->embedded.print_token();
                                     },
                                     [](acc::node::UnaryExpr* uexpr) {},
                                     [](acc::node::GroupingExpr* gexpr) {}},
                   expr);
    };
    std::vector<acc::ExprVariant> parse() {
        while (!is_end()) {
            exprs.push_back(parse_primary());
        };
        return exprs;
    };
    acc_parser(const acc_parser&) = delete;
    acc_parser& operator=(const acc_parser&) = delete;
    acc_parser(acc_parser&&) = default;
    acc_parser& operator=(acc_parser&&) = default;
    ~acc_parser() = default;
};
}  // namespace acc