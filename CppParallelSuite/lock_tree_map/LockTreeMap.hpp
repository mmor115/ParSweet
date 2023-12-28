
#ifndef LOCKTREEMAP_HPP
#define LOCKTREEMAP_HPP

#include "../KeyType.hpp"
#include "../Types.hpp"
#include "Node.hpp"


namespace parallel_suite::lock_tree_map {

    template <OrderedKeyType K, typename V, typename Mutex=std::mutex>
    class LockTreeMap {
    private:
        using MyNode = Node<K, V, Mutex>;

        std::shared_ptr<MyNode> head;

        bool get(K key, V& outVal, std::shared_ptr<MyNode> node) {
            if (!node) {
                return false;
            }

            std::unique_lock selfLock(node->selfMutex);

            auto cmp = key <=> node->key;

            if (cmp == 0) {
                outVal = node->value;
                return true;
            } else if (cmp < 0) {
                std::lock_guard leftLock(node->leftMutex);
                selfLock.unlock();
                return get(key, outVal, node->left);
            } else {
                std::lock_guard rightLock(node->rightMutex);
                selfLock.unlock();
                return get(key, outVal, node->right);
            }
        }

        std::shared_ptr<MyNode> findInsertionParent(K key,
                                                    std::shared_ptr<MyNode> node,
                                                    std::shared_ptr<MyNode> parent,
                                                    std::unique_lock<Mutex>& theLock) {
            if (!node) {
                if (parent) {
                    theLock = std::unique_lock(parent->selfMutex);
                }
                return parent;
            }

            std::unique_lock selfLock(node->selfMutex);

            auto cmp = key <=> node->key;

            if (cmp < 0) {
                std::lock_guard leftLock(node->leftMutex);
                selfLock.unlock();
                return findInsertionParent(key, node->left, node, theLock);
            } else {
                std::lock_guard rightLock(node->rightMutex);
                selfLock.unlock();
                return findInsertionParent(key, node->right, node, theLock);
            }
        }

        std::shared_ptr<MyNode> min(std::shared_ptr<MyNode> node) {
            std::scoped_lock locks(node->selfMutex, node->leftMutex);

            if (!node->left) {
                return node;
            } else {
                return min(node->left);
            }
        }

        std::shared_ptr<MyNode> del_(K key, std::shared_ptr<MyNode> node) {
            if (!node) {
                return node;
            }

            std::unique_lock selfLock(node->selfMutex);

            auto cmp = key <=> node->key;

            if (cmp < 0) {
                std::lock_guard leftLock(node->leftMutex);
                selfLock.unlock();
                node->left = del_(key, node->left);
                return node;
            } else if (cmp > 0) {
                std::lock_guard rightLock(node->rightMutex);
                selfLock.unlock();
                node->right = del_(key, node->right);
                return node;
            } else {
                if (!(node->left)) {
                    return node->right;
                } else if (!(node->right)) {
                    return node->left;
                } else {
                    std::scoped_lock locks(node->rightMutex, node->right->selfMutex);
                    std::shared_ptr<MyNode> successor = min(node->right);
                    node->key = successor->key;
                    node->value = successor->value;

                    std::shared_ptr<MyNode> successorSuccessor = del_(node->key, node->right);
                    node->right->key = successorSuccessor->key;
                    node->right->value = successorSuccessor->value;

                    return node;
                }
            }
        }
    public:
        LockTreeMap() : head(nullptr) { }

        bool get(K key, V& outVal) {
            return get(key, outVal, head);
        }

        void put(K key, V val) {
            std::shared_ptr<MyNode> newNode = std::make_shared<MyNode>(key, val);

            std::unique_lock<Mutex> insertionParentLock;
            std::shared_ptr<MyNode> parent = findInsertionParent(key, head, nullptr, insertionParentLock);

            if (!parent) {
                head = newNode;
            } else {
                if ((key <=> parent->key) < 0) {
                    std::lock_guard leftLock(parent->leftMutex);
                    parent->left = newNode;
                } else {
                    std::lock_guard rightLock(parent->rightMutex);
                    parent->right = newNode;
                }
            }
        }

        bool del(K key, V& outValue) {
            std::shared_ptr<MyNode> n = del_(key, head);
            if (n) {
                outValue = n->value;
                return true;
            } else {
                return false;
            }
        }
    };
} // parallel_suite::lock_tree_map

#endif //LOCKTREEMAP_HPP
