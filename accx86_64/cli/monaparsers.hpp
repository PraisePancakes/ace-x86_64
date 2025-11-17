#pragma once
#include "../fnl/pmonadic.hpp"
namespace acc::mona {

class {
   public:
    static auto operator()() {
        return acc::transform_(acc::many_(acc::either_1(acc::transform_(acc::sequ_(acc::letters_(), acc::match_('/')),
                                                                        [](auto seq) {
                                                                            return std::get<0>(seq) + std::get<1>(seq);
                                                                        }),
                                                        acc::transform_(acc::sequ_(acc::match_(".."), acc::match_('/')),
                                                                        [](auto seq) {
                                                                            return std::get<0>(seq) + std::get<1>(seq);
                                                                        }))),
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
                                   acc::letters_("Extension type not supported")),
                               [](auto seq) {
                                   return std::apply([](auto&&... tuple_args) {
                                       return (tuple_args + ...);
                                   },
                                                     seq);
                               });
    }
} constexpr file_parser;
}  // namespace acc::mona