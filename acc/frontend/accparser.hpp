#pragma once
#include <iostream>
#include <vector>

#include "../traits/token_trait.hpp"
#include "../utils/visitor.hpp"
#include "accast.hpp"
#include "acctoken.hpp"
#include "storage.hpp"
#define DISCARD(f) (void)f
namespace acc {

template <typename T = acc::token>
class [[nodiscard]] acc_parser
    : public acc::storage<std::vector<T>> {
    std::vector<acc::ExprVariant> exprs;  // block will have this

    bool check_it(acc::ACC_ALL_TOKEN_ENUM type) noexcept {
        if (this->is_end()) {
            return false;
        }
        return this->peek().type == type;
    }
    bool match_it(acc::ACC_ALL_TOKEN_ENUM type) noexcept {
        if (check_it(type)) {
            DISCARD(this->advance());
            return true;
        }

        return false;
    }

    auto match_any(traits::acc_token_t auto&&... types) {
        return (match_it(types) || ...);
    }
    /*
    TOKEN TYPE ID (xF) [TK_LITERAL_INT]
        location (row, col) < 1 , 0 > [INT]     123
    TOKEN TYPE ID (xF) [TK_LITERAL_STRING]
        location (row, col) < 3 , 0 > [STRING] "456"
    TOKEN TYPE ID (\0) [TK_EOF]
        location (row, col) < 0 , 0 > [CHAR]   '\0'
    */
    acc::ExprVariant parse_primary() {
        if (match_any(TK_LITERAL_INT,
                      TK_LITERAL_STRING,
                      TK_LITERAL_DOUBLE,
                      TK_LITERAL_FLOAT)) {
            return new acc::node::LiteralExpr{.value = this->peek_prev().value,
                                              .embedded = this->peek_prev()};
        };

        throw std::runtime_error("PARSERROR");
    }

   public:
    acc_parser(const std::vector<T>& toks)
        : acc::storage<std::vector<T>>(toks) {

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
        do {
            exprs.push_back(parse_primary());
        } while (!this->is_end());
        return exprs;
    };
    acc_parser(const acc_parser&) = delete;
    acc_parser& operator=(const acc_parser&) = delete;
    acc_parser(acc_parser&&) = default;
    acc_parser& operator=(acc_parser&&) = default;
    ~acc_parser() = default;
};
}  // namespace acc