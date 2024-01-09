
#ifndef BACKOFF_LOCK_HPP
#define BACKOFF_LOCK_HPP

#include <atomic>
#include <chrono>
#include <thread>
#include "../Rand.hpp"

namespace parallel_suite::locks {

    template <int MinDelay = 1, int MaxDelay = 17>
    class BackoffLock {
    private:
        std::atomic<bool> aBool;
    public:
        BackoffLock() : aBool(false) { }

        void lock() {
            int delayLimit = MinDelay;
            IntRand rand;

            for (;;) {
                while (aBool.load()) { }

                if (!aBool.exchange(true)) {
                    return;
                } else {
                    auto delay = std::chrono::milliseconds(rand.getRand(0, delayLimit));
                    delayLimit = std::min(MaxDelay, 2 * delayLimit);
                    std::this_thread::sleep_for(delay);
                }
            }
        }

        void unlock() {
            aBool.store(false);
        }
    };
}

#endif //BACKOFF_LOCK_HPP
