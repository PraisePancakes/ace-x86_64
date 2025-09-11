#pragma once
#include <iostream>
#include <unordered_map>

#include "tkxmacro.hpp"
namespace acc::globals {
const static std::unordered_map<std::string, acc::GLOBAL_TOKENS> ACC_PAIR_DELIMS{{"!=", acc::GLOBAL_TOKENS::TK_BANG_EQ},
                                                                                 {">=", acc::GLOBAL_TOKENS::TK_GT_EQ},
                                                                                 {"<=", acc::GLOBAL_TOKENS::TK_LT_EQ},
                                                                                 {"==", acc::GLOBAL_TOKENS::TK_STRICT_EQ}};
}