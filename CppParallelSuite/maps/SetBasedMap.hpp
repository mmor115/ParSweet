
#ifndef SET_BASED_MAP_HPP
#define SET_BASED_MAP_HPP

#include <mutex>
#include "../sets/SetTypes.hpp"
#include "../KeyType.hpp"
#include "../Types.hpp"

namespace parallel_suite::maps {

    template <KeyType K, typename V>
    class SetBasedMapNode {
    public:
        K key;
        V value;

        SetBasedMapNode(K key, V value) : key(key), value(value) { }
        SetBasedMapNode(K key) : key(key) { } // NOLINT(*-explicit-constructor)

        bool operator==(SetBasedMapNode const& rhs) const {
            return key == rhs.key;
        }

        bool operator!=(SetBasedMapNode const& rhs) const {
            return key != rhs.key;
        }
    };

    template <KeyType K, typename V, template<typename, typename...> class Set, usize NumBuckets = 16, typename... SetArgs>
    class SetBasedMap {
    private:
        using MyNode = SetBasedMapNode<K, V>;
        using MySet = Set<MyNode, SetArgs...>;
        static_assert(sets::SetType<MySet, MyNode>);

        std::array<std::unique_ptr<MySet>, NumBuckets> buckets;

        MySet* getNodeHead(usize index) {
            return buckets[index].get();
        }

        usize getBucketIndexOfKey(K key) {
            return std::hash<K>{}(key) % (NumBuckets - 1);
        }
    public:
        SetBasedMap() : buckets() {
            for (usize i = 0; i < NumBuckets; ++i) {
                buckets[i] = std::make_unique<MySet>();
            }
        }

        bool put(K key, V value) {
            auto index = getBucketIndexOfKey(key);
            MyNode node(key, value);
            return getNodeHead(index)->add(node);
        }

        bool get(K key, V& outValue) {
            auto index = getBucketIndexOfKey(key);

            std::optional<MyNode> n = getNodeHead(index)->getEqual(key);

            if (n) {
                outValue = n->value;
                return true;
            } else {
                return false;
            }
        }

        bool del(K key) {
            auto index = getBucketIndexOfKey(key);
            return getNodeHead(index)->remove(key);
        }
    };
} // parallel_suite::maps

namespace std {
    template <typename K, typename V>
    struct hash<parallel_suite::maps::SetBasedMapNode<K, V>> { // NOLINT(*-dcl58-cpp)
        usize operator()(parallel_suite::maps::SetBasedMapNode<K, V> const& n) const {
            return hash<K>{}(n.key);
        }
    };
} // std

#endif //SET_BASED_MAP_HPP
