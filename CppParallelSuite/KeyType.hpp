
#ifndef KEYTYPE_HPP
#define KEYTYPE_HPP

#include "Types.hpp"
#include <concepts>
#include <memory>

namespace parallel_suite {
    template <typename K>
    concept KeyType = std::equality_comparable<K> && requires(K k) {
        { std::hash<K>{}(k) } -> std::convertible_to<usize>;
    };

    template <typename K>
    concept OrderedKeyType = std::totally_ordered<K> && std::three_way_comparable<K> && KeyType<K>;
} // namespace parallel_suite

#endif // KEYTYPE_HPP
