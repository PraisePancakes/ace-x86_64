#pragma once
#include <variant>

#include "x86ops.hpp"

namespace acc::x86_64isa {

struct Memory {
};

struct Register {
};

class Operand {
    std::variant<unsigned, Register, Memory> value;
};

struct add;
struct mov;
struct jz;
struct jmp;
struct push;
struct call;
struct cmp;
struct je;
struct jne;
struct jnz;

using InstructionVariant = std::variant<add, mov, jz, jmp, push, call, cmp, je, jne, jnz>;

struct add {
    Operand destination;
    Operand source;
};

struct mov {
    Operand destination;
    Operand source;
};

struct jz {
    Operand destination;
};
struct je {
    Operand destination;
};

struct jne {
    Operand destination;
};

struct jnz {
    Operand destination;
};

struct push {
    Operand what;
};

struct jmp {
    Operand destination;
};

struct call {
    Operand operand;
};

}  // namespace acc::x86_64isa