
#ifndef FINE_GRAINED_SET_HPP
#define FINE_GRAINED_SET_HPP

#include "../KeyType.hpp"
#include "../MutexType.hpp"
#include "../Types.hpp"
#include "NodeMarkers.hpp"
#include "SetNode.hpp"
#include "SetTypes.hpp"
#include <mutex>

namespace parallel_suite::sets {

    template <KeyType T, MutexType Mutex = std::mutex>
    class FineGrainedSet {
    private:
        using MyNode = AtomicNode<T, Mutex>;

        std::shared_ptr<MyNode> head;

        template <FindCallback<MyNode> F>
        bool find(T const& t, F&& callback) {
            const auto key = std::hash<T>{}(t);

            std::unique_lock predecessorLock(head->mutex);
            std::shared_ptr<MyNode> predecessor = head;

            std::unique_lock currentLock(predecessor->next.load()->mutex);
            std::shared_ptr<MyNode> current = predecessor->next;

            while (current->next.load() && current->key <= key && (key != current->key || t != current->value)) {
                predecessor = current;
                current = current->next;
                predecessorLock.swap(currentLock);
                currentLock = std::unique_lock(current->mutex);
            }

            auto r = callback(predecessor, current);
            currentLock.unlock();
            predecessorLock.unlock();
            return r;
        }

    public:
        FineGrainedSet() : head(std::make_shared<MyNode>(HeadNode)) {
            head->next = std::make_shared<MyNode>(TailNode);
        }

        bool contains(T const& t) {
            return find(t, [&t](auto predecessor, auto current) {
                return t == current->value;
            });
        }

        std::optional<T> getEqual(T const& t) {
            std::optional<T> ret;

            find(t, [&t, &ret](auto predecessor, auto current) {
                if (t != current->value) {
                    return false;
                }

                ret = current->value;
                return true;
            });

            return ret;
        }

        bool add(T t) {
            return find(t, [&t](auto predecessor, auto current) {
                if (t == current->value) {
                    return false;
                }

                auto newNode = std::make_shared<MyNode>(t);
                newNode->next = predecessor->next.load();
                predecessor->next = std::move(newNode);
                return true;
            });
        }

        bool remove(T const& t) {
            return find(t, [&t](auto predecessor, auto current) {
                if (t != current->value) {
                    return false;
                }

                predecessor->next = std::move(current->next.load());
                return true;
            });
        }
    };

} // namespace parallel_suite::sets


#endif //FINE_GRAINED_SET_HPP
