
#ifndef LOCKHASHMAP_HPP
#define LOCKHASHMAP_HPP

#include <mutex>
#include <array>
#include <condition_variable>
#include "../KeyType.hpp"
#include "../Types.hpp"
#include "../MutexType.hpp"

namespace parallel_suite::maps {

    template <KeyType K, typename V, usize NumBuckets, MutexType Mutex=std::mutex>
    class LockHashMap {
        class Node {
        public:
            const K key;
            V value;
            std::unique_ptr<Node> next;

            Node(K key, V value) : key(key), value(value), next(nullptr) { }
        };

        class NodeHead {
        public:
            Mutex mutex;
            std::unique_ptr<Node> head;

            NodeHead() : mutex(), head(nullptr) { }
        };

    private:
        using MyNodeHead = NodeHead;
        using MyNode = Node;

        std::array<std::unique_ptr<MyNodeHead>, NumBuckets> buckets;

        MyNodeHead* getNodeHead(usize index, std::unique_lock<Mutex>& lock) {
            lock = std::unique_lock(buckets[index]->mutex);
            return buckets[index].get();
        }

        usize getBucketIndexOfKey(K key) {
            return std::hash<K>{}(key) % (NumBuckets - 1);
        }

    public:
        LockHashMap() : buckets() {
            for (usize i = 0; i < NumBuckets; ++i) {
                buckets[i] = std::make_unique<MyNodeHead>();
            }
        }

        bool put(K key, V value, V& oldValue) {
            auto index = getBucketIndexOfKey(key);

            std::unique_lock<Mutex> headLock{};
            MyNodeHead* nodeHead = getNodeHead(index, headLock);

            MyNode* h = nodeHead->head.get();
            if (!h) {
                nodeHead->head = std::make_unique<MyNode>(key, value);
            } else {
                MyNode* current = h;

                while (true) {
                    if (current->key == key) {
                        oldValue = current->value;
                        current->value = value;
                        return true;
                    }

                    if (current->next) {
                        current = current->next.get();
                    } else {
                        break;
                    }
                }

                current->next = std::make_unique<MyNode>(key, value);
            }

            return false;
        }

        bool get(K key, V& outValue) {
            auto index = getBucketIndexOfKey(key);

            std::unique_lock<Mutex> headLock{};
            auto nodeHead = getNodeHead(index, headLock);

            MyNode* h = nodeHead->head.get();
            if (!h) {
                return false;
            }

            MyNode* current = h;

            while (current) {
                if (current->key == key) {
                    outValue = current->value;
                    return true;
                }
                current = current->next.get();
            }

            return false;
        }

        bool del(K key) {
            auto index = getBucketIndexOfKey(key);

            std::unique_lock<Mutex> headLock{};
            MyNodeHead* nodeHead = getNodeHead(index, headLock);

            MyNode* h = nodeHead->head.get();
            if (!h) {
                return false;
            }

            MyNode* current = h;
            MyNode* prev = nullptr;
            while (current) {
                if (current->key == key) {
                    if (!prev) {
                        nodeHead->head = std::move(current->next);
                    } else {
                        prev->next = std::move(current->next);
                    }

                    return true;
                }

                prev = current;
                current = current->next.get();
            }

            return false;
        }

    };
} // parallel_suite::maps


#endif //LOCKHASHMAP_HPP
