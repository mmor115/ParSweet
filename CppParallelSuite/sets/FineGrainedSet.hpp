
#ifndef FINE_GRAINED_SET_HPP
#define FINE_GRAINED_SET_HPP

#include "../Types.hpp"
#include "../KeyType.hpp"
#include "../MutexType.hpp"
#include "NodeMarkers.hpp"
#include "SetNode.hpp"
#include "SetTypes.hpp"

namespace parallel_suite::sets {

    template <OrderedKeyType T, MutexType Mutex=std::mutex>
    class FineGrainedSet {
    private:
        using MyNode = Node<T, Mutex>;

        std::shared_ptr<MyNode> head;

        template<bool ReleaseCurrent = false, FindCallback<MyNode> F>
        bool find(T const& t, F&& callback) {
            const auto key = std::hash<T>{}(t);

            std::unique_lock predecessorLock(head->mutex);
            std::unique_lock currentLock(head->next->mutex);

            std::shared_ptr<MyNode> predecessor = head;
            std::shared_ptr<MyNode> current = head->next;

            while (current->next.get() != nullptr
                    && current->key <= key
                    && (key != current->key || t != current->value)) {
                predecessor = current;
                current = current->next;
                predecessorLock.swap(currentLock);
                currentLock = std::unique_lock(current->mutex);
            }

            auto changeHappened = callback(predecessor.get(), current.get());

            if constexpr (ReleaseCurrent) {
                if (changeHappened) {
                    currentLock.release();
                }
            }

            return changeHappened;
        }

    public:
        FineGrainedSet() : head(std::make_shared<MyNode>(HeadNode)) {
            head->next = std::make_shared<MyNode>(TailNode);
        }

        bool contains(T const& t) {
            return find(t, [&t](auto* predecessor, auto* current) {
                return t == current->value;
            });
        }

        std::optional<T> getEqual(T const& t) {
            std::optional<T> ret;

            find(t, [&t, &ret](auto* predecessor, auto* current) {
                if (t != current->value) {
                    return false;
                }

                ret = current->value;
                return true;
            });

            return ret;
        }

        bool add(T t) {
            return find(t, [&t](auto* predecessor, auto* current) {
                if (t == current->value) {
                    return false;
                }

                auto newNode = std::make_shared<MyNode>(t);
                newNode->next.swap(predecessor->next);
                predecessor->next = std::move(newNode);
                return true;
            });
        }

        bool remove(T const& t) {
            return find<true>(t, [&t](auto* predecessor, auto* current) {
                if (t != current->value) {
                    return false;
                }

                predecessor->next = std::move(current->next);
                return true;
            });
        }
    };

}// parallel_suite::sets


#endif //FINE_GRAINED_SET_HPP
