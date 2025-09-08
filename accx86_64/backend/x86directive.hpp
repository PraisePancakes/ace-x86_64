#pragma once

namespace acc::x86_64isa {
enum class DIRECTIVES {
    DATA,
    DB,
    DW,
    DD,
    DUP,
    // size directives
    // https://www.cs.virginia.edu/~evans/cs216/guides/x86.html#:~:text=In%20general%2C%20the,4%20bytes%20respectively.
    BYTE_PTR,  // 1 byte
    WORD_PTR,  // 2 bytes
    DWORD_PTR  // 4 bytes
};
}