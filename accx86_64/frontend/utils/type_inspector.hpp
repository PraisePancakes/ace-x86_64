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
    [[nodiscard]] static std::pair<std::optional<acc::types::TYPES>, acc::token> to_type(acc::token key) {
        auto it = std::find_if(acc::globals::token_map.begin(), acc::globals::token_map.end(), [&key](auto p) {
            if (key.type == TK_RESERVED_TYPE) {
                return (key.word == std::visit(internal::visitor{
                                                   [](const std::string s) { return s; },
                                                   [](const char c) { return std::string{c}; }},
                                               p.first));
            }
            return (key.type == (p.second.flags >> acc::globals::TOKEN_TYPE_SHIFTER));
        });
        return std::make_pair(it->second.deduced_type, key);
    };

    [[nodiscard]] static std::string to_string(std::optional<acc::types::TYPES> key) {
        if (key) {
            auto it = std::find_if(acc::globals::token_map.begin(), acc::globals::token_map.end(), [&key](auto p) {
                if (p.second.deduced_type.has_value()) {
                    auto kind = p.second.flags >> acc::globals::TOKEN_TYPE_SHIFTER;
                    if ((kind == TK_RESERVED_TYPE) || (kind == TK_IDENTIFIER && p.second.deduced_type == types::TYPES::CLASS))
                        return p.second.deduced_type == key;
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

    [[nodiscard]] static std::string to_string(acc::node::info::type_info* base, std::string str = "") {
        if (base) {
            std::visit(internal::visitor{
                           [&str](const std::pair<types::TYPES, acc::token> type) {
                               str += type.second.word;
                           },
                           [&str](const acc::node::info::Pointer type) {
                               if (type.pointee) {
                                   str += to_string(type.pointee, " Pointer to ");
                               }
                           },
                           [&str](const acc::node::info::Array array) {
                               if (array.element) {
                                   str += to_string(array.element, " size " + std::to_string(array.len) + " of ");
                               }
                           }},
                       base->type);
            return str;
        }
        return "unknown";
    }
};
}  // namespace acc::utils