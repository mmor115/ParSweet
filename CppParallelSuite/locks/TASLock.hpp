
#ifndef TASLOCK_HPP
#define TASLOCK_HPP

#include "LockTraits.hpp"
#include <atomic>
#include <thread>

namespace parallel_suite::locks {
    class TASLock {
    private:
        std::atomic<bool> aBool;

    public:
        TASLock() : aBool(false) {}

        void lock() {
            while (aBool.exchange(true)) {
                std::this_thread::yield();
            }
        }

        void unlock() {
            aBool.store(false);
        }
    };

    template <>
    struct LockTraits<TASLock> {
        constexpr static char const* name = "TASLock";
    };
} // namespace parallel_suite::locks

#endif //TASLOCK_HPP
