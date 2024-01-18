
#ifndef FINE_GRAINED_SET_HPP
#define FINE_GRAINED_SET_HPP

#include "../Types.hpp"
#include "../KeyType.hpp"
#include "../MutexType.hpp"
#include "NodeMarkers.hpp"
#include "SetNode.hpp"
#include "SetTypes.hpp"

namespace parallel_suite::sets {

    template <KeyType T, MutexType Mutex=std::mutex>
    class FineGrainedSet {
    private:
        using MyNode = Node<T, Mutex>;

        std::unique_ptr<MyNode> head;

        template<FindCallback<MyNode> F>
        bool find(T const& t, F&& callback) {
            const auto key = std::hash<T>{}(t);

            std::unique_lock predecessorLock(head->mutex);
            std::unique_lock currentLock(head->next->mutex);

            auto* predecessor = &head;
            auto* current = &head->next;

            while ((**current).next != nullptr
                    && (**current).key <= key
                    && (key != (**current).key || t != (**current).value)) {
                predecessor = current;
                current = &((**current).next);
                predecessorLock.swap(currentLock);
                currentLock = std::unique_lock((**current).mutex);
            }

            return callback(predecessor->get(), current->get());
        }

    public:
        FineGrainedSet() : head(std::make_unique<MyNode>(HeadNode)) {
            head->next = std::make_unique<MyNode>(TailNode);
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

                auto newNode = std::make_unique<MyNode>(t);
                newNode->next.swap(predecessor->next);
                predecessor->next = std::move(newNode);
                return true;
            });
        }

        bool remove(T const& t) {
            return find(t, [&t](auto* predecessor, auto* current) {
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
