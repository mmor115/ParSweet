
#ifndef JLOCK_HPP
#define JLOCK_HPP

#include <atomic>
#include <thread>
#include "../Types.hpp"
#include "../threadlocal/ThreadId.hpp"

namespace parallel_suite::locks {
    using namespace threadlocal;

    class JLock {
    private:
        std::atomic<usize> turn;
    public:
        JLock() : turn(0) { }

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
}

#endif //JLOCK_HPP
