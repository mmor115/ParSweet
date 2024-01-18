
#ifndef SET_NODE_HPP
#define SET_NODE_HPP

#include "../Types.hpp"
#include "../KeyType.hpp"
#include "../MutexType.hpp"
#include "NodeMarkers.hpp"
#include <optional>
#include <stdint.h>

namespace parallel_suite::sets {

    template <KeyType T, MutexType Mutex>
    class Node {
    public:
        Mutex mutex;
        usize const key;
        std::optional<T> const value;
        std::unique_ptr<Node> next;

        explicit Node(T value) : mutex(), key(std::hash<T>{}(value)), value(value), next(nullptr) { }
        explicit Node(HeadNode_t) : mutex(), key(0), value(std::nullopt), next(nullptr) { }
        explicit Node(TailNode_t) : mutex(), key(SIZE_MAX), value(std::nullopt), next(nullptr) { }

        void lock() {
            mutex.lock();
        }

        void unlock() {
            mutex.unlock();
        }
    };

    template <KeyType T, MutexType Mutex>
    class MarkableNode : Node<T, Mutex> {
    public:
        bool deleted;

        explicit MarkableNode(T value) : Node<T, Mutex>(value), deleted(false) { }
        explicit MarkableNode(HeadNode_t) : Node<T, Mutex>(HeadNode), deleted(false) { }
        explicit MarkableNode(TailNode_t) : Node<T, Mutex>(TailNode), deleted(false) { }
    };
} // parallel_suite::sets

#endif //SET_NODE_HPP
