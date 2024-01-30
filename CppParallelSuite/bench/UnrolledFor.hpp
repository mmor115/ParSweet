
#ifndef UNROLLED_FOR_HPP
#define UNROLLED_FOR_HPP

#include <type_traits>

template <auto From, auto To>
constexpr void unrolledFor(auto&& fn) {
    if constexpr (From < To) {
        fn(std::integral_constant<decltype(From), From>());
        unrolledFor<From + 1, To>(fn);
    }
}

template <typename Arg, typename... Args>
constexpr void formatSpecific(std::string& base, Arg&& arg, Args&&... args) {
    base += "<";
    base += std::to_string(arg);
    ((base += ":" + std::to_string(args)), ...);
    base += ">";
}

#endif //UNROLLED_FOR_HPP
