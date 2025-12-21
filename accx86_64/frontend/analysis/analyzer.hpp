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
        using acc::types::type_checker;
        std::visit(internal::visitor{
                       [](const acc::node::DeclarationStmt* stmt) {
                           if (stmt->type) {
                               auto* variable_type_info = stmt->type;
                               if (stmt->expr.has_value()) {
                                   auto* expression_type_info = type_checker::evaluate_type(stmt->expr.value());
                                   if(variable_type_info != expression_type_info) {
                                    
                                   }
                               }
                           }
                       },
                       [](const acc::node::IfStmt* stmt) {},
                       [](const acc::node::WhileStmt* stmt) {},
                       [](const acc::node::BlockStmt* stmt) {},
                       [](const acc::node::ForStmt* stmt) {},
                       [](const acc::node::ExpressionStmt* stmt) {},
                       [](const acc::node::FuncStmt* stmt) {},
                       [](const acc::node::ReturnStmt* stmt) {},
                       [](const acc::node::TypeStmt* stmt) {},
                       [](std::monostate) {}},
                   variant);
    };

    void report_error(const acc::exceptions::type_error& error) const noexcept {
        std::stringstream msg;
        msg << error.what << " with types being \n";
        for (auto const& t : error.inconvertible_types) {
            msg << acc::utils::type_inspector::to_string(t) << " , " << std::endl;
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