
#ifndef LOCKHASHMAP_NODEHEAD_HPP
#define LOCKHASHMAP_NODEHEAD_HPP


#include <mutex>
#include <condition_variable>
#include "Node.hpp"
#include "../KeyType.hpp"

namespace parallel_suite::lock_hash_map {

    template <KeyType K, typename V, typename Mutex=std::mutex>
    class NodeHead {
    public:
        Mutex mutex;
        std::shared_ptr<Node<K, V>> head;

        NodeHead() : mutex(), head(nullptr) { }
    };

} // parallel_suite::lock_hash_map

#endif //LOCKHASHMAP_NODEHEAD_HPP
