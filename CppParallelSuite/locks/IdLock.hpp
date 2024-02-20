
#ifndef JLOCK_HPP
#define JLOCK_HPP

#include "../Types.hpp"
#include "../threadlocal/ThreadId.hpp"
#include "LockTraits.hpp"
#include <atomic>
#include <thread>

namespace parallel_suite::locks {
    using namespace threadlocal;

    class IdLock {
    private:
        std::atomic<usize> turn;

    public:
        IdLock() : turn(0) {}

        void lock() {
            const auto ticket = ThreadId::get();

            for (;;) {
                usize zero = 0;
                if (turn.compare_exchange_weak(zero, ticket)) {
                    return;
                }

                std::this_thread::yield();
            }
        }

        void unlock() {
            const auto ticket = ThreadId::get();

            for (;;) {
                auto dummy = ticket;
                if (turn.compare_exchange_weak(dummy, 0)) {
                    return;
                }

                std::this_thread::yield();
            }
        }
    };

    template <>
    struct LockTraits<IdLock> {
        constexpr static char const* name = "IdLock";
    };
} // namespace parallel_suite::locks

#endif //JLOCK_HPP
