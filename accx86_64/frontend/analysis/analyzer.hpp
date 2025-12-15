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
                           type_checker::evaluate_type(stmt->expr);
                           analyze_types(stmt->body);
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
                           analyze_types(stmt->environment);
                       },
                       [this](const acc::node::BlockStmt* stmt) {
                           for (const auto& item : stmt->env->get_items()) {
                               analyze_types(item);
                           }
                           if (stmt->ret) {
                               analyze_types(stmt->ret);
                           }
                       },
                       [this](const acc::node::IfStmt* stmt) {
                           auto condition_type = type_checker::evaluate_type(stmt->condition);
                           if (!type_checker::check_valid_implicit_conversion(condition_type, types::TYPES::BOOL)) {
                               throw type_error({condition_type, types::TYPES::BOOL}, " conditional failed to deduce to a boolean type ");
                           }
                           analyze_types(stmt->then);
                           if (stmt->else_.has_value()) {
                               analyze_types(stmt->else_.value());
                           }
                       },
                       [this](const acc::node::WhileStmt* stmt) {
                           auto condition_type = type_checker::evaluate_type(stmt->condition);
                           if (!type_checker::check_valid_implicit_conversion(condition_type, types::TYPES::BOOL)) {
                               throw type_error({condition_type, types::TYPES::BOOL}, " conditional failed to deduce to a boolean type ");
                           }
                           analyze_types(stmt->body);
                       },
                       [this](const acc::node::ReturnStmt* stmt) {
                           type_checker::evaluate_type(stmt->expr);
                       },
                       [](std::monostate) { std::unreachable(); }},
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