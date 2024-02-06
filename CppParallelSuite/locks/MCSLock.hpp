
#ifndef MCS_LOCK_HPP
#define MCS_LOCK_HPP


#include <atomic>
#include <thread>
#include "../threadlocal/ThreadLocal.hpp"
#include "LockTraits.hpp"

namespace parallel_suite::locks {
    using namespace parallel_suite::threadlocal;

    namespace mcs {
        class QNode {
        public:
            std::atomic<bool> locked;
            std::atomic<QNode*> next;
            QNode() : locked(false), next(nullptr) { }
        };
    }

    class MCSLock {
    private:
        ThreadLocal<mcs::QNode*> myNode;
        std::atomic<mcs::QNode*> tail;

    public:
        MCSLock() : myNode(), tail(nullptr) { }

        ~MCSLock() {
            delete tail.load();
        }

        void lock() {
            if (!myNode.isSet()) {
                myNode.set(new mcs::QNode());
            }

            mcs::QNode* qNode = myNode.get();
            mcs::QNode* predecessor = tail.exchange(qNode);

            if (predecessor) {
                qNode->locked = true;
                predecessor->next = qNode;
                while (qNode->locked.load()) {
                    std::this_thread::yield();
                }
            }
        }

        void unlock() {
            mcs::QNode* qNode = myNode.get();
            if (!qNode->next.load()) {
                mcs::QNode* dummy = qNode;
                if (tail.compare_exchange_strong(dummy, nullptr)) {
                    return;
                }
                while (!qNode->next.load()) {
                    std::this_thread::yield();
                }
            }

            qNode->next.load()->locked = false;
            qNode->next = nullptr;
        }
    };

    template <>
    struct LockTraits<MCSLock> {
        constexpr static char const* name = "MCSLock";
    };
}

#endif //MCS_LOCK_HPP
