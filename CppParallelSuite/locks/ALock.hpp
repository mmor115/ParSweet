
#ifndef ALOCK_HPP
#define ALOCK_HPP

#include <atomic>
#include <thread>
#include <array>
#include "../threadlocal/ThreadLocal.hpp"
#include "../Types.hpp"
#include "LockTraits.hpp"

namespace parallel_suite::locks {
    using namespace threadlocal;

    template <usize ThreadCount>
    class ALock {
    private:
        ThreadLocal<usize> mySlotIndex;
        std::atomic<usize> tail;
        std::array<std::atomic<bool>, ThreadCount> flags;

    public:
        ALock() : mySlotIndex(), tail(0), flags() {
            for (int i = 0; i < ThreadCount; ++i) {
                flags[i] = (i == 0);
            }
        }

        void lock() {
            auto slot = tail.fetch_add(1) % ThreadCount;
            mySlotIndex.set(slot);
            while (!flags[slot]) {
                std::this_thread::yield();
            }
            flags[slot] = false;
        }

        void unlock() {
            auto slot = mySlotIndex.get();
            auto next = (slot + 1) % ThreadCount;
            flags[next] = true;
        }
    };

    template <usize ThreadCount>
    struct LockTraits<ALock<ThreadCount>> {
        constexpr static char const* name = "ALock";
    };
}

#endif //ALOCK_HPP
