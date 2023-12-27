
#ifndef LOCKHASHMAP_HPP
#define LOCKHASHMAP_HPP

#include <mutex>
#include <array>
#include <condition_variable>
#include "NodeHead.hpp"
#include "../KeyType.hpp"
#include "../Types.hpp"

namespace parallel_suite::lock_hash_map {

    template <KeyType K, typename V, usize NumBuckets, typename Mutex=std::mutex>
    class LockHashMap {
    private:
        using MyNodeHead = NodeHead<K, V, Mutex>;
        using MyNode = Node<K, V, Mutex>;

        std::array<std::shared_ptr<MyNodeHead>, NumBuckets> buckets;

        std::shared_ptr<MyNodeHead> getNodeHead(usize index) {
            return buckets[index];
        }

        usize getBucketIndexOfKey(K key) {
            return std::hash<K>{}(key) % NumBuckets - 1;
        }

    public:
        LockHashMap() : buckets() {
            for (usize i = 0; i < NumBuckets; ++i) {
                buckets[i] = std::make_shared<MyNodeHead>();
            }
        }

        bool put(K key, V value, V& oldValue) {
            auto index = getBucketIndexOfKey(key);
            std::shared_ptr<MyNode> newNode = std::make_shared<MyNode>(key, value);
            std::shared_ptr<MyNodeHead> nodeHead = getNodeHead(index);

            std::lock_guard headLock(nodeHead->mutex);

            std::shared_ptr<MyNode> h = nodeHead->head;
            if (!h) {
                nodeHead->head = newNode;
            } else {
                std::shared_ptr<MyNode> current = h;

                while (true) {
                    if (current->key == key) {
                        oldValue = current->value;
                        current->value = value;
                        return true;
                    }

                    if (current->next) {
                        current = current->next;
                    } else {
                        break;
                    }
                }

                current->next = newNode;
            }

            return false;
        }

        bool get(K key, V& outValue) {
            auto index = getBucketIndexOfKey(key);
            auto nodeHead = getNodeHead(index);

            std::lock_guard headLock(nodeHead->mutex);

            std::shared_ptr<MyNode> h = nodeHead->head;
            if (!h) {
                return false;
            }

            std::shared_ptr<MyNode> current = h;

            while (current) {
                if (current->key == key) {
                    outValue = current->value;
                    return true;
                }
                current = current->next;
            }

            return false;
        }

        bool del(K key) {
            auto index = getBucketIndexOfKey(key);
            std::shared_ptr<MyNodeHead> nodeHead = getNodeHead(index);

            std::lock_guard headLock(nodeHead->mutex);

            std::shared_ptr<MyNode> h = nodeHead->head;
            if (!h) {
                return false;
            }

            std::shared_ptr<MyNode> current = h, prev = {nullptr};
            while (current) {
                if (current->key == key) {
                    if (!prev) {
                        nodeHead->head = current->next;
                    } else {
                        prev->next = current->next;
                    }

                    return true;
                }

                prev = current;
                current = current->next;
            }

            return false;
        }

    };
} // parallel_suite::lock_hash_map


#endif //LOCKHASHMAP_HPP
