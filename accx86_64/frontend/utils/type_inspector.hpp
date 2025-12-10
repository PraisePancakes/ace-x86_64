#pragma once
#include <algorithm>
#include <variant>

#include "../acctoken.hpp"
#include "../analysis/typesys/types.hpp"
#include "../statics/ro_tokenmap.hpp"

namespace acc::utils {
class type_inspector {
   public:
    [[nodiscard]] static std::optional<acc::types::TYPES> to_type(acc::token key) {
        auto it = std::find_if(acc::globals::token_map.begin(), acc::globals::token_map.end(), [&key](auto p) {
            return p.second.type == key.type;
        });
        return it->second.deduced_type;
    };
};
}  // namespace acc::utils