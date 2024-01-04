
#ifndef TASLOCK_HPP
#define TASLOCK_HPP

#include <atomic>
#include <thread>

namespace parallel_suite::locks {
    class TASLock {
    private:
        std::atomic<bool> aBool;

    public:
        TASLock() : aBool(false) { }

        void lock() {
            while (aBool.exchange(true)) {
                std::this_thread::yield();
            }
        }

        void unlock() {
            aBool.store(false);
        }
    };
}

#endif //TASLOCK_HPP
