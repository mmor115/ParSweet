
#ifndef LOCKHASHMAP_NODE_HPP
#define LOCKHASHMAP_NODE_HPP


#include <mutex>
#include <condition_variable>
#include "../KeyType.hpp"

namespace parallel_suite::lock_hash_map {

    template <KeyType K, typename V, typename Mutex=std::mutex>
    class Node {
    public:
        const K key;
        V value;
        std::shared_ptr<Node<K, V, Mutex>> next;

        Node(K key, V value) : key(key), value(value), next(nullptr) { }
    };

} // parallel_suite::lock_hash_map

#endif //LOCKHASHMAP_NODE_HPP
