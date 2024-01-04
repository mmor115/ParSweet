
#ifndef ILOCK_HPP
#define ILOCK_HPP

#include <atomic>
#include <thread>
#include "../Types.hpp"
#include "../threadlocal/ThreadLocal.hpp"

namespace parallel_suite::locks {
    using namespace threadlocal;

    class ILock {
    private:
        std::atomic<usize> turn;
        ThreadLocal<usize> myTicket;
        static std::atomic<usize> ticketCounter;

    public:
        ILock() : turn(0), myTicket() { }

        void lock() {
            auto ticket = ticketCounter.fetch_add(1);
            myTicket.set(ticket);

            for (;;) {
                usize zero = 0;
                if (turn.compare_exchange_weak(zero, ticket)) {
                    return;
                }

                std::this_thread::yield();
            }
        }

        void unlock() {
            auto ticket = myTicket.get();

            for (;;) {
                if (turn.compare_exchange_weak(ticket, 0)) {
                    return;
                }

                std::this_thread::yield();
            }
        }
    };
}

#endif //ILOCK_HPP
