
#ifndef KEYTYPE_HPP
#define KEYTYPE_HPP

#include <concepts>
#include <memory>
#include "Types.hpp"

namespace parallel_suite {
    template<typename K>
    concept KeyType = std::totally_ordered<K> && requires(K k) {
        { std::hash<K>{}(k) } -> std::convertible_to<usize>;
    };
}

#endif // KEYTYPE_HPP
