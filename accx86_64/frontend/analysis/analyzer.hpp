#pragma once
#include "../accenv.hpp"
#include "typesys/integral_types.hpp"
#include "typesys/types.hpp"
namespace acc {
class analyzer {
    using environment_t = acc::environment<std::string, acc::StmtVariant>;
    environment_t* env;
    static std::optional<acc::types::INTEGRAL_TYPES> type_word_to_integral_type(acc::token type) noexcept {
        using acc::types::INTEGRAL_TYPES;
        if (type.word == "int") {
            return INTEGRAL_TYPES::INT;
        } else if (type.word == "bool") {
            return INTEGRAL_TYPES::BOOL;
        } else if (type.word == "char") {
            return INTEGRAL_TYPES::CHAR;
        } else if (type.word == "short") {
            return INTEGRAL_TYPES::SHORT;
        } else if (type.word == "long") {
            return INTEGRAL_TYPES::LONG;
        } else if (type.word == "long long") {
            return INTEGRAL_TYPES::LONG_LONG;
        }
        return std::nullopt;
    };
    void analyze_types(const acc::StmtVariant variant) {
        using acc::exceptions::type_error;

        using acc::types::type_checker;

        std::visit(internal::visitor{
                       [](acc::node::DeclarationStmt* stmt) {
                           if (stmt->expr) {
                               if (!type_checker::check_valid_implicit_conversion(type_word_to_integral_type(stmt->type).value(),
                                                                                  type_checker::evaluate_type(stmt->expr.value()))) {
                                   throw type_error({type_word_to_integral_type(stmt->type).value(),
                                                     type_checker::evaluate_type(stmt->expr.value())},
                                                    "incompatible types ");
                               }
                           }
                       },
                       [](const acc::node::ExpressionStmt* stmt) {
                           type_checker::evaluate_type(stmt->expr);
                       },
                       [this](const acc::node::ForStmt* stmt) {
                           analyze_types(stmt->init);
                           analyze_types(stmt->condition);

                           analyze_types(stmt->body);
                       },
                       [this](const acc::node::WhileStmt* stmt) {
                           analyze_types(stmt->body);
                           type_checker::evaluate_type(stmt->condition);
                       },
                       [this](const acc::node::FuncStmt* stmt) {
                           analyze_types(stmt->body);
                           auto Tfunc = type_checker::token_to_integral_type(stmt->type.type);
                           auto Tret = type_checker::evaluate_type(stmt->body->ret->expr);
                           if (!type_checker::check_valid_implicit_conversion(Tfunc.value(), Tret)) {
                               throw type_error({Tfunc.value(), Tret}, "mismatch return types between function declaration and definition. ");
                           }
                       },
                       /*kept here only to silence */ [](auto) {}},
                   variant);
    };

    void report_error(const acc::exceptions::type_error& error) const noexcept {
        std::stringstream msg;
        msg << error.what << " with types being \n";
        for (auto const& t : error.inconvertible_types) {
            msg << std::to_underlying(t) << " , " << std::endl;
        }

        acc::logger::instance().send(acc::logger::LEVEL::ERROR, msg.str());
    };

   public:
    analyzer(environment_t* environment) : env(environment) {};
    void try_analyze() {
        try {
            for (const auto& i : env->get_items()) {
                analyze_types(i);
            }
        } catch (acc::exceptions::type_error const& te) {
            report_error(te);
        }
    };
};
}  // namespace acc