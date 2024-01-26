
#ifndef TJLOCK_HPP
#define TJLOCK_HPP

#include <atomic>
#include <thread>
#include "../Types.hpp"
#include "../threadlocal/ThreadId.hpp"

namespace parallel_suite::locks {
    using namespace threadlocal;

    class TJLock {
    private:
        std::atomic<usize> turn;
    public:
        TJLock() : turn(0) { }

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
}

#endif //TJLOCK_HPP
