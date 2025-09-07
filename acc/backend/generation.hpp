#pragma once
#include <iostream>

#include "../frontend/accparser.hpp"
namespace acc::x86_64isa {
class generator {
    std::string generated;

   public:
    generator(std::vector<acc::ExprVariant> ast_nodes) {};
};
};  // namespace acc::x86_64isa