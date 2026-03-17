#pragma once
#include <iostream>

#include "../frontend/accparser.hpp"
#include "instruction.hpp"
namespace acc::x86_64isa {
class generator {
    std::vector<InstructionVariant> instructions;

   public:
    generator(std::vector<acc::StmtVariant> ast_nodes) {

    };
};
};  // namespace acc::x86_64isa