#include "Bench.hpp"

#include <vector>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <functional>

#if HAVE_HPX
  #include <hpx/hpx.hpp>
  #include <hpx/hpx_main.hpp>
#endif
#include "../MutexType.hpp"
#include "../locks/ALock.hpp"
#include "../locks/OptimizedALock.hpp"
#include "../locks/IdLock.hpp"
#include "../locks/TIdLock.hpp"
#include "../locks/TASLock.hpp"
#include "../locks/TTASLock.hpp"
#include "../locks/BackoffLock.hpp"
#include "../locks/CLHLock.hpp"
#include "../locks/MCSLock.hpp"
#include "../locks/TwoCounterLock.hpp"
#include "BlackBox.hpp"

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

        blackBox(counter);
    }

#if HAVE_HPX
    template <MutexType M>
    void benchHpxLock(usize const& nThreads, usize const& countTo) {
        std::vector<hpx::thread> workers;

        usize counter = 0;
        M theMutex;

        for (int threadId = 0; threadId < nThreads; ++threadId) {
            workers.emplace_back(work<M>, std::cref(countTo), std::ref(counter), std::ref(theMutex));
        }

        for (auto&& worker : workers) {
            worker.join();
        }

        blackBox(counter);
    }
#endif

} // parallel_bench::locks


using namespace parallel_bench;
using namespace parallel_bench::locks;

template <typename L>
constexpr auto benchLockWrapper() {
    return [](BenchParameters const& params) {
        benchLock<L>(params.getNThreads(), params.getWorkPerThread());
    };
}

#if HAVE_HPX
template <typename L>
constexpr auto benchHpxLockWrapper() {
    return [](BenchParameters const& params) {
        benchHpxLock<L>(params.getNThreads(), params.getWorkPerThread());
    };
}
#endif

std::unordered_map<std::string, std::function<void(BenchParameters const&)>> tests {
        {"ALock<100>", benchLockWrapper<ALock<100>>()},
        {"OptimizedALock<100>", benchLockWrapper<OptimizedALock<100>>()},
        {"BackoffLock<1:17>", benchLockWrapper<BackoffLock<1, 17>>()},
        {"IdLock", benchLockWrapper<IdLock>()},
        {"TIdLock", benchLockWrapper<TIdLock>()},
        {"CLHLock", benchLockWrapper<CLHLock>()},
        {"MCSLock", benchLockWrapper<MCSLock>()},
        {"TASLock", benchLockWrapper<TASLock>()},
        {"TTASLock", benchLockWrapper<TTASLock>()},
        {"TwoCounterLock", benchLockWrapper<TwoCounterLock>()},
        {"std::mutex", benchLockWrapper<std::mutex>()},
        {"std::recursive_mutex", benchLockWrapper<std::recursive_mutex>()}
#if HAVE_HPX
        ,{"hpx::mutex", benchHpxLockWrapper<hpx::mutex>()}
#endif
};

int main() {
    BenchParameters params("c++", "locks");

    auto which = params.getWhich();
    if (which && tests.contains(*which)) {
        writeBenchResult(params, *which, measure(tests[*which], params));
        params.coolOff();
    } else {
        for (auto& pair : tests) {
            writeBenchResult(params, pair.first, measure(pair.second, params));
            params.coolOff();
        }
    }

    return 0;
}
