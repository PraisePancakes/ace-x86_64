#pragma once
#include "../accenv.hpp"
#include "../utils/type_inspector.hpp"
#include "typesys/type_checker.hpp"
#include "typesys/type_error.hpp"
#include "typesys/types.hpp"
namespace acc {
class analyzer {
    using environment_t = acc::environment<std::string, acc::StmtVariant>;
    environment_t* env;

    void analyze_types(const acc::StmtVariant variant) {
        using acc::exceptions::type_error;
        using acc::types::type_checker;
        using acc::utils::type_inspector;

        std::visit(internal::visitor{
                       [](acc::node::DeclarationStmt* stmt) {
                           if (stmt->expr) {
                               std::cout << stmt->type.word << (int)type_inspector::to_type(stmt->type).value();
                               if (!type_checker::check_valid_implicit_conversion(type_inspector::to_type(stmt->type).value(),
                                                                                  type_checker::evaluate_type(stmt->expr.value()))) {
                                   throw type_error({type_inspector::to_type(stmt->type).value(),
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
                           auto Tfunc = type_inspector::to_type(stmt->type);
                           auto Tret = type_checker::evaluate_type(stmt->body->ret->expr);
                           if (!type_checker::check_valid_implicit_conversion(Tfunc.value(), Tret)) {
                               throw type_error({Tfunc.value(), Tret}, "mismatch return types between function declaration and definition. ");
                           }
                       },
                       [this](const acc::node::TypeStmt* stmt) {

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