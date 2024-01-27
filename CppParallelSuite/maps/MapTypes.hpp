
#ifndef MAP_TYPES_HPP
#define MAP_TYPES_HPP

#include <concepts>
#include <optional>

namespace parallel_suite::maps {
    template <typename M, typename K, typename V>
    concept MapType = std::is_default_constructible_v<M> && requires(M s, K k, V v, V& out) {
        { s.get(k, out) } -> std::convertible_to<bool>;
        { s.put(k, v, out) } -> std::convertible_to<bool>;
        { s.del(k) } -> std::convertible_to<bool>;
    };
} // parallel_suite::maps

#endif //MAP_TYPES_HPP
