#pragma once
#include "../accenv.hpp"
#include "typesys/types.hpp"
namespace acc {
class analyzer {
    using environment_t = acc::environment<std::string, acc::StmtVariant>;
    environment_t* env;
    void analyze_types(const acc::StmtVariant variant) {
        using acc::exceptions::type_error;
        using acc::types::type_checker;

        std::visit(internal::visitor{
                       [](const acc::node::DeclarationStmt* stmt) {
                           if (stmt->expr) {
                               if (type_checker::token_to_integral_type(stmt->type.type) != type_checker::evaluate_type(stmt->expr.value())) {
                                   throw type_error({type_checker::token_to_integral_type(stmt->type.type).value(),
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
                       /*kept here only to silence */ [](auto) {}},
                   variant);
    };

   public:
    analyzer(environment_t* environment) : env(environment) {};
};
}  // namespace acc