#pragma once
#include "../fnl/pmonadic.hpp"
namespace acc::monadic {

class {
   public:
    static auto operator()() {
        return acc::transform_(acc::many_(acc::transform_(acc::sequ_(acc::letters_(), acc::match_('/')),
                                                          [](auto seq) {
                                                              return std::get<0>(seq) + std::get<1>(seq);
                                                          })),
                               [](auto many) {
                                   std::string prefix = "";
                                   for (auto each : many.first) {
                                       std::cout << "EACH : " << each << "\n";
                                       prefix += each;
                                   }
                                   return prefix;
                               });
    }
} constexpr prefix_parser;

class {
   public:
    static auto operator()() {
        return acc::transform_(acc::sequ_(
                                   prefix_parser(),
                                   acc::letters_(),
                                   acc::match_('.'),
                                   acc::match_("acc", "Ace cannot translate a file type that does not implement the .acc extension")),
                               [](auto seq) {
                                   return std::apply([](auto&&... tuple_args) {
                                       return (tuple_args + ...);
                                   },
                                                     seq);
                               });
    }
} constexpr file_parser;
}  // namespace acc::monadic