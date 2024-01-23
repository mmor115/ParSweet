
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
        std::shared_ptr<Node> next;

        explicit Node(T value) : mutex(), key(std::hash<T>{}(value)), value(value), next(nullptr) { }
        explicit Node(HeadNode_t) : mutex(), key(0), value(std::nullopt), next(nullptr) { }
        explicit Node(TailNode_t) : mutex(), key(SIZE_MAX), value(std::nullopt), next(nullptr) { }
    };

    template <KeyType T, MutexType Mutex>
    class AtomicNode {
    public:
        Mutex mutex;
        usize const key;
        std::optional<T> const value;
        std::atomic<std::shared_ptr<AtomicNode>> next;

        explicit AtomicNode(T value) : mutex(), key(std::hash<T>{}(value)), value(value), next(nullptr) { }
        explicit AtomicNode(HeadNode_t) : mutex(), key(0), value(std::nullopt), next(nullptr) { }
        explicit AtomicNode(TailNode_t) : mutex(), key(SIZE_MAX), value(std::nullopt), next(nullptr) { }
    };

    template <KeyType T, MutexType Mutex>
    class MarkableNode {
    public:
        Mutex mutex;
        usize const key;
        std::optional<T> const value;
        std::shared_ptr<MarkableNode> next;
        bool deleted;

        explicit MarkableNode(T value) : mutex(), key(std::hash<T>{}(value)), value(value), next(nullptr), deleted(false) { }
        explicit MarkableNode(HeadNode_t) : mutex(), key(0), value(std::nullopt), next(nullptr), deleted(false) { }
        explicit MarkableNode(TailNode_t) : mutex(), key(SIZE_MAX), value(std::nullopt), next(nullptr), deleted(false) { }
    };

    template <KeyType T, MutexType Mutex>
    class AtomicMarkableNode {
    public:
        Mutex mutex;
        usize const key;
        std::optional<T> const value;
        std::atomic<std::shared_ptr<AtomicMarkableNode>> next;
        std::atomic<bool> deleted;

        explicit AtomicMarkableNode(T value) : mutex(), key(std::hash<T>{}(value)), value(value), next(nullptr), deleted(false) { }
        explicit AtomicMarkableNode(HeadNode_t) : mutex(), key(0), value(std::nullopt), next(nullptr), deleted(false) { }
        explicit AtomicMarkableNode(TailNode_t) : mutex(), key(SIZE_MAX), value(std::nullopt), next(nullptr), deleted(false) { }
    };
} // parallel_suite::sets

#endif //SET_NODE_HPP
