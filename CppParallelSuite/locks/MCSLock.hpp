
#ifndef MCS_LOCK_HPP
#define MCS_LOCK_HPP


#include <atomic>
#include <thread>
#include "../threadlocal/ThreadLocal.hpp"

namespace parallel_suite::locks {
    using namespace parallel_suite::threadlocal;

    namespace mcs {
        class QNode {
        public:
            std::atomic<bool> locked;
            QNode* next;
            QNode() : locked(false), next(nullptr) { }
        };
    }

    class MCSLock {
    private:
        ThreadLocal<mcs::QNode> myNode;
        std::atomic<mcs::QNode*> tail;

    public:
        MCSLock() : myNode(), tail(nullptr) { }

        ~MCSLock() {
            delete tail.load();
        }

        void lock() {
            if (!myNode.isSet()) {
                myNode.init();
            }

            mcs::QNode* qNode = myNode.getPtr();
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
            mcs::QNode* qNode = myNode.getPtr();
            if (!qNode->next) {
                mcs::QNode* dummy = qNode;
                if (tail.compare_exchange_strong(dummy, nullptr)) {
                    return;
                }
                while (!qNode->next) {
                    std::this_thread::yield();
                }
            }

            qNode->next->locked = false;
            qNode->next = nullptr;
        }
    };
}

#endif //MCS_LOCK_HPP