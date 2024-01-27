
#ifndef TWO_COUNTER_LOCK_HPP
#define TWO_COUNTER_LOCK_HPP

#include <atomic>
#include <thread>
#include "../Types.hpp"

namespace parallel_suite::locks {
    class TwoCounterLock {
    private:
        std::atomic<usize> currentTicket;
        std::atomic<usize> ticketCounter;

    public:
        TwoCounterLock() : currentTicket(0), ticketCounter(0) { }

        void lock() {
            auto ticket = ticketCounter.fetch_add(1);

            while (currentTicket.load() != ticket) {
                std::this_thread::yield();
            }
        }

        void unlock() {
            currentTicket.fetch_add(1);
        }
    };
} // parallel_suite::locks

#endif //TWO_COUNTER_LOCK_HPP
