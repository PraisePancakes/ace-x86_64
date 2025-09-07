#pragma once

namespace acc::x86_64isa {
enum class REGISTERS {
    // eax is 32-bit with ax = 16 bit, ah = 8 bit and al = 8 bit
    EAX,
    AX,
    AH,
    AL,
    // ebx is 32-bit with bx = 16 bit, bh = 8 bit and bl = 8 bit
    EBX,
    BX,
    BH,
    BL,
    // ecx is 32-bit with cx = 16 bit, ch = 8 bit and cl = 8 bit
    ECX,
    CX,
    CH,
    CL,
    // edx is 32-bit with dx = 16 bit, dh = 8 bit and dl = 8 bit
    EDX,
    DX,
    DH,
    DL,
    // 32 bit registers
    ESI,
    EDI,
    // reserved 32 bit registers
    ESP,
    EBP
};
}