#include "Bench.hpp"

#include <vector>
#include <thread>
#include <mutex>
#include "../MutexType.hpp"
#include "../locks/ALock.hpp"
#include "../locks/IdLock.hpp"
#include "../locks/TIdLock.hpp"
#include "../locks/TASLock.hpp"
#include "../locks/TTASLock.hpp"
#include "../locks/BackoffLock.hpp"
#include "../locks/CLHLock.hpp"
#include "../locks/MCSLock.hpp"
#include "../locks/TwoCounterLock.hpp"

namespace parallel_bench::locks {
    using namespace parallel_suite;
    using namespace parallel_suite::locks;

    template <MutexType M>
    void work(usize const& countTo, usize& counter, M& theMutex) {
        for (int c = 0; c < countTo; ++c) {
            std::lock_guard lock(theMutex);
            ++counter;
        }
    }

    template <MutexType M>
    void benchLock(usize const& nThreads, usize const& countTo) {
        std::vector<std::thread> workers;

        usize counter = 0;
        M theMutex;

        for (int threadId = 0; threadId < nThreads; ++threadId) {
            workers.emplace_back(work<M>, std::cref(countTo), std::ref(counter), std::ref(theMutex));
        }

        for (auto&& worker : workers) {
            worker.join();
        }
    }
} // parallel_bench::locks

using namespace parallel_bench;
using namespace parallel_bench::locks;

int main() {
    BenchParameters params("c++", "locks");

    writeBenchResult(params, "ALock<100>", measure([&params]() {
        benchLock<ALock<100>>(params.getNThreads(), params.getWorkPerThread());
    }));

    writeBenchResult(params, "BackoffLock<1:17>", measure([&params]() {
        benchLock<BackoffLock<1, 17>>(params.getNThreads(), params.getWorkPerThread());
    }));

    writeBenchResult(params, "CLHLock", measure([&params]() {
        benchLock<CLHLock>(params.getNThreads(), params.getWorkPerThread());
    }));

    writeBenchResult(params, "IdLock", measure([&params]() {
        benchLock<IdLock>(params.getNThreads(), params.getWorkPerThread());
    }));

    writeBenchResult(params, "MCSLock", measure([&params]() {
        benchLock<MCSLock>(params.getNThreads(), params.getWorkPerThread());
    }));

    writeBenchResult(params, "TASLock", measure([&params]() {
        benchLock<TASLock>(params.getNThreads(), params.getWorkPerThread());
    }));

    writeBenchResult(params, "TIdLock", measure([&params]() {
        benchLock<TIdLock>(params.getNThreads(), params.getWorkPerThread());
    }));

    writeBenchResult(params, "TTASLock", measure([&params]() {
        benchLock<TTASLock>(params.getNThreads(), params.getWorkPerThread());
    }));

    writeBenchResult(params, "TwoCounterLock", measure([&params]() {
        benchLock<TwoCounterLock>(params.getNThreads(), params.getWorkPerThread());
    }));

    writeBenchResult(params, "std::mutex", measure([&params]() {
        benchLock<std::mutex>(params.getNThreads(), params.getWorkPerThread());
    }));

    writeBenchResult(params, "std::recurive_mutex", measure([&params]() {
        benchLock<std::recursive_mutex>(params.getNThreads(), params.getWorkPerThread());
    }));

    return 0;
}
