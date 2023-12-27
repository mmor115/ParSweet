
#ifndef KEYTYPE_HPP
#define KEYTYPE_HPP

#include <concepts>
#include <memory>
#include "Types.hpp"

namespace parallel_suite {
    template <typename K>
    concept KeyType = std::equality_comparable<K> && requires(K k) {
        { std::hash<K>{}(k) } -> std::convertible_to<usize>;
    };

    template <typename K>
    concept OrderedKeyType = std::totally_ordered<K> && KeyType<K>;
}

#endif // KEYTYPE_HPP
