#pragma once
#include <algorithm>
#include <variant>

#include "../acctoken.hpp"
#include "../analysis/typesys/types.hpp"
#include "../printers/token_printer.hpp"
#include "../statics/ro_tokenmap.hpp"
#include "lexeme_inspector.hpp"
namespace acc::utils {
class type_inspector {
   public:
    [[nodiscard]] static std::optional<acc::types::TYPES> to_type(acc::token key) {
        acc::output::token_printer::write_token(std::cout, key);
        auto it = std::find_if(acc::globals::token_map.begin(), acc::globals::token_map.end(), [&key](auto p) {
            if (key.type == TK_RESERVED_TYPE) {
                return (key.word == std::visit(internal::visitor{
                                                   [](const std::string s) { return s; },
                                                   [](const char c) { return std::string{c}; }},
                                               p.first));
            }
            return (key.type == (p.second.flags >> acc::globals::TOKEN_TYPE_SHIFTER));
        });
        return it->second.deduced_type;
    };

    [[nodiscard]] static std::string to_string(std::optional<acc::types::TYPES> key) {
        if (key) {
            auto it = std::find_if(acc::globals::token_map.begin(), acc::globals::token_map.end(), [&key](auto p) {
                if (p.second.deduced_type.has_value()) {
                    return p.second.deduced_type == key && ((p.second.flags >> acc::globals::TOKEN_TYPE_SHIFTER) == TK_RESERVED_TYPE);
                }
                return false;
            });

            return std::visit(internal::visitor{
                                  [](const std::string s) { return s; },
                                  [](const char c) { return std::to_string(c); }},
                              it->first);
        }
        return "unknown";
    }
};
}  // namespace acc::utils