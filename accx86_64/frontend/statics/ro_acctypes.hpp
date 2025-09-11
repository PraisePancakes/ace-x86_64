#pragma once
#include <iostream>
#include <unordered_set>

namespace acc::globals {
const static std::unordered_set<std::string> ACC_TYPE_SET{"int",
                                                          "float",
                                                          "char",
                                                          "bool",
                                                          "short",
                                                          "long",
                                                          "double"};
}