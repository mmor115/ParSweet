#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include "../locks/ALock.hpp"
#include "../locks/ILock.hpp"
#include "../locks/TASLock.hpp"
#include "../locks/TTASLock.hpp"
#include "../MutexType.hpp"
#include "../locks/BackoffLock.hpp"
#include "../locks/CLHLock.hpp"
#include "../locks/MCSLock.hpp"

namespace parallel_bench::locks {
    using namespace parallel_suite;
    using namespace parallel_suite::locks;

    constexpr static int Threads = 12;
    constexpr static int CountTo = 20000;

    template <MutexType M>
    void work(int threadId, usize& counter, M& theMutex) {
        for (int c = 0; c < CountTo; ++c) {
            std::lock_guard lock(theMutex);
            ++counter;
        }
    }

    template <MutexType M>
    bool testLock() {
        std::vector<std::thread> workers;

        usize counter = 0;
        M theMutex;

        for (int threadId = 0; threadId < Threads; ++threadId) {
            workers.emplace_back(work<M>, threadId, std::ref(counter), std::ref(theMutex));
        }

        for (auto&& worker : workers) {
            worker.join();
        }

        auto target = Threads * CountTo;

        return target == counter;
    }
} // parallel_bench::locks

using namespace parallel_bench::locks;

template <MutexType... Mutices>
void testLocksRepeatedly() {
    constexpr static int Tries = 30;
    bool ok = true;

    for (int t = 0; t < Tries; ++t) {
        ok = ok && (... && testLock<Mutices>());
    }

    if (ok) {
        std::cout << "All tests passed." << std::endl;
    } else {
        std::cout << "A test failed!" << std::endl;
    }
}

int main() {
    testLocksRepeatedly<
            std::mutex,
            std::recursive_mutex,
            TASLock,
            TTASLock,
            ALock<Threads>,
            ILock,
            BackoffLock<>,
            CLHLock,
            MCSLock
    >();
    return 0;
}
