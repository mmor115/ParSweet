
#ifndef SET_TYPES_HPP
#define SET_TYPES_HPP

#include <concepts>
#include <optional>

namespace parallel_suite::sets {
    template <typename S, typename E>
    concept SetType = std::is_default_constructible_v<S> && requires(S s, E e) {
        { s.add(e) } -> std::convertible_to<bool>;
        { s.remove(e) } -> std::convertible_to<bool>;
        { s.contains(e) } -> std::convertible_to<bool>;
        { s.getEqual(e) } -> std::same_as<std::optional<E>>;
    };

    template <typename F, typename N>
    concept FindCallback = requires(F f, std::shared_ptr<N> n1, std::shared_ptr<N> n2) {
        f(n1, n2);
    };

    template <typename F, typename N>
    concept PtrFindCallback = requires(F f, N* n1, N* n2) {
        f(n1, n2);
    };
} // namespace parallel_suite::sets

#endif //SET_TYPES_HPP
