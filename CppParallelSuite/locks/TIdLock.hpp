
#ifndef TJLOCK_HPP
#define TJLOCK_HPP

#include "../Types.hpp"
#include "../threadlocal/ThreadId.hpp"
#include "LockTraits.hpp"
#include <atomic>
#include <thread>

namespace parallel_suite::locks {
    using namespace threadlocal;

    class TIdLock {
    private:
        std::atomic<usize> turn;

    public:
        TIdLock() : turn(0) {}

        void lock() {
            const auto ticket = ThreadId::get();

            for (;;) {
                usize zero = 0;
                if (turn.load() == 0 && turn.compare_exchange_weak(zero, ticket)) {
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
    struct LockTraits<TIdLock> {
        constexpr static char const* name = "TIdLock";
    };
} // namespace parallel_suite::locks

#endif //TJLOCK_HPP
