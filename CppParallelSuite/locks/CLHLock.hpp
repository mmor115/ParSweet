
#ifndef CLH_LOCK_HPP
#define CLH_LOCK_HPP


#include <atomic>
#include <thread>
#include "../threadlocal/ThreadLocal.hpp"

namespace parallel_suite::locks {
    using namespace parallel_suite::threadlocal;

    namespace clh {
        class QNode {
        public:
            bool locked;
            QNode() : locked(false) { }
        };
    }

    class CLHLock {
    private:
        ThreadLocal<clh::QNode> myPredecessor;
        ThreadLocal<clh::QNode> myNode;
        std::atomic<clh::QNode*> tail;

    public:
        CLHLock() : myPredecessor(), myNode(), tail(new clh::QNode()) { }

        ~CLHLock() {
            delete tail.load();
        }

        void lock() {
            if (!myNode.isSet()) {
                myNode.init();
            }

            clh::QNode* qNode = myNode.getPtr();
            qNode->locked = true;

            clh::QNode* predecessor = tail.exchange(qNode);

            myPredecessor.setPtr(predecessor);

            while (predecessor->locked) {
                std::this_thread::yield();
            }
        }

        void unlock() {
            clh::QNode* qNode = myNode.getPtr();
            qNode->locked = false;
            myNode.setPtr(myPredecessor.getPtr());
            myPredecessor.setPtr(nullptr);
        }
    };
}

#endif //CLH_LOCK_HPP
