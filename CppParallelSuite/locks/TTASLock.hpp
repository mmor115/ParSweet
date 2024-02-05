
#ifndef TTASLOCK_HPP
#define TTASLOCK_HPP

#include <atomic>
#include <thread>
#include "LockTraits.hpp"

namespace parallel_suite::locks {
    class TTASLock {
    private:
        std::atomic<bool> aBool;

    public:
        TTASLock() : aBool(false) { }

        void lock() {
            for (;;) {
                if (!aBool.load() && !aBool.exchange(true)) {
                    return;
                }

                std::this_thread::yield();
            }
        }

        void unlock() {
            aBool.store(false);
        }
    };

    template <>
    struct LockTraits<TTASLock> {
        constexpr static char const* name = "TTASLock";
    };
}

#endif //TTASLOCK_HPP
