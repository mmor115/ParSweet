
#ifndef NODE_HPP
#define NODE_HPP

#include <mutex>
#include <condition_variable>
#include "../KeyType.hpp"

namespace parallel_suite::lock_tree_map {

    template <OrderedKeyType K, typename V, typename Mutex=std::mutex>
    class Node {
    public:
        K key;
        V value;
        std::shared_ptr<Node<K, V, Mutex>> left, right;
        Mutex selfMutex, leftMutex, rightMutex;

        Node(K key, V value) : key(key), value(value), left(nullptr), right(nullptr), selfMutex(), leftMutex(), rightMutex() { }
    };

} // parallel_suite::lock_tree_map

#endif //NODE_HPP
