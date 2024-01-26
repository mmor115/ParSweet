
#ifndef THREADID_HPP
#define THREADID_HPP

#include <atomic>
#include <cassert>
#include <iostream>
#include "../Types.hpp"

namespace parallel_suite::threadlocal {
    class ThreadId {
    private:
        static std::atomic<usize> idCounter;
        static thread_local usize id;
    public:
        static usize get() {
            if (id == 0)  {
                id = idCounter.fetch_add(1);
            }

            assert(id != 0);
            return id;
        }
    };
} // parallel_suite::threadlocal

#endif //THREADID_HPP
