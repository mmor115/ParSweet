
#ifndef CLH_LOCK_HPP
#define CLH_LOCK_HPP


#include <atomic>
#include <thread>
#include <memory>
#include "../threadlocal/ThreadLocal.hpp"

namespace parallel_suite::locks {
    using namespace parallel_suite::threadlocal;

    class QNode {
    public:
        bool locked;
        QNode() : locked(false) { }
    };

    class CLHLock {
    private:
        ThreadLocal<QNode> myPredecessor;
        ThreadLocal<QNode> myNode;
        std::atomic<QNode*> tail;

    public:
        CLHLock() : myPredecessor(), myNode(), tail(new QNode()) { }

        ~CLHLock() {
            QNode* qNode = tail.load();
            delete qNode;
        }

        void lock() {
            if (!myNode.isSet()) {
                myNode.init();
            }

            QNode* qNode = myNode.getPtr();
            qNode->locked = true;

            QNode* predecessor = tail.exchange(qNode);

            myPredecessor.setPtr(predecessor);

            while (predecessor->locked) {
                std::this_thread::yield();
            }
        }

        void unlock() {
            QNode* qNode = myNode.getPtr();
            qNode->locked = false;
            myNode.setPtr(myPredecessor.getPtr());
            myPredecessor.setPtr(nullptr);
        }
    };
}

#endif //CLH_LOCK_HPP
