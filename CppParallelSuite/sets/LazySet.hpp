
#ifndef LAZY_SET_HPP
#define LAZY_SET_HPP

#include <mutex>
#include "../Types.hpp"
#include "../KeyType.hpp"
#include "../MutexType.hpp"
#include "NodeMarkers.hpp"
#include "SetNode.hpp"
#include "SetTypes.hpp"

namespace parallel_suite::sets {

    template <KeyType T, MutexType Mutex=std::mutex>
    class LazySet {
    private:
        using MyNode = AtomicMarkableNode<T, Mutex>;

        std::shared_ptr<MyNode> head;

        template<FindCallback<MyNode> F>
        bool find(T const& t, F&& callback) {
            const auto key = std::hash<T>{}(t);

            for (;;) {
                std::shared_ptr<MyNode> predecessor = head;
                std::shared_ptr<MyNode> current = predecessor->next;

                while (current->next.load()
                       && current->key <= key
                       && (key != current->key || t != current->value)) {
                    predecessor = current;
                    current = current->next;
                }

                assert(current);
                assert(predecessor);

                std::unique_lock predecessorLock(predecessor->mutex, std::defer_lock);
                std::unique_lock currentLock(current->mutex, std::defer_lock);

                std::lock(predecessorLock, currentLock);

                if (!predecessor->deleted && !current->deleted && predecessor->next.load() == current) {
                    return callback(predecessor, current);
                }
            }
        }

    public:
        LazySet() : head(std::make_shared<MyNode>(HeadNode)) {
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
                current->deleted = true;
                predecessor->next = current->next.load();
                return true;
            });
        }
    };

}// parallel_suite::sets


#endif //LAZY_SET_HPP
