#pragma once
#include <unordered_map>
#include <unordered_set>

#include "tkxmacro.hpp"
namespace acc::globals {
const static std::unordered_set<acc::GLOBAL_TOKENS> ACC_DELIMS{acc::GLOBAL_TOKENS::TK_SPACE,
                                                               acc::GLOBAL_TOKENS::TK_PLUS,
                                                               acc::GLOBAL_TOKENS::TK_DASH,
                                                               acc::GLOBAL_TOKENS::TK_STAR,
                                                               acc::GLOBAL_TOKENS::TK_BANG,
                                                               acc::GLOBAL_TOKENS::TK_PAREN_L,
                                                               acc::GLOBAL_TOKENS::TK_PAREN_R,
                                                               acc::GLOBAL_TOKENS::TK_BANG,
                                                               acc::GLOBAL_TOKENS::TK_SEMI,
                                                               acc::GLOBAL_TOKENS::TK_COLON,
                                                               acc::GLOBAL_TOKENS::TK_LT,
                                                               acc::GLOBAL_TOKENS::TK_GT,
                                                               acc::GLOBAL_TOKENS::TK_EQUALS};
}