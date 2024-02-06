#include "Bench.hpp"

#include <vector>
#include <future>
#if HAVE_HPX
#include <hpx/hpx.hpp>
#include <hpx/hpx_main.hpp>
#endif
#include "../sets/FineGrainedSet.hpp"
#include "../locks/LockTraits.hpp"
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

namespace parallel_suite::locks {
    template <>
    struct LockTraits<std::mutex> {
        constexpr static char const* name = "std::mutex";
    };

    template <>
    struct LockTraits<std::recursive_mutex> {
        constexpr static char const* name = "std::recursive_mutex";
    };
#if HAVE_HPX
    template <>
    struct LockTraits<hpx::mutex> {
        constexpr static char const* name = "hpx::mutex";
    };

    template <>
    struct LockTraits<hpx::spinlock> {
        constexpr static char const* name = "hpx::spinlock";
    };
#endif
}

namespace parallel_bench::sets {
    using namespace parallel_suite;
    using namespace parallel_suite::sets;

    template <SetType<int> IntSet>
    void benchSet(usize const& nThreads, usize const& workSize) {
        std::vector<std::future<void>> futures;

        IntSet theSet;

        for (int threadId = 0; threadId < nThreads; ++threadId) {
            futures.push_back(std::async(std::launch::async, [threadId, &theSet, &workSize, &nThreads]() {
                for (int i = 0; i < workSize; ++i) {
                    auto token = i * nThreads + threadId;

                    blackBox(theSet.add(token));
                    blackBox(theSet.contains(token));
                    blackBox(theSet.remove(token));
                    blackBox(theSet.contains(token));
                }

                for (int i = 0; i < workSize; i++) {
                    blackBox(theSet.add(i * nThreads + threadId));
                }

                for (int i = 0; i < workSize; i++) {
                    blackBox(theSet.contains(i * nThreads + threadId));
                }

                for (int i = 0; i < workSize; i++) {
                    blackBox(theSet.remove(i * nThreads + threadId));
                }

                for (int i = 0; i < workSize; i++) {
                    blackBox(theSet.contains(i * nThreads + threadId));
                }
            }));
        }

        for (auto&& worker : futures) {
            worker.get();
        }
    }

    #if HAVE_HPX
    template <SetType<int> IntSet>
    void benchHpxSet(usize const& nThreads, usize const& workSize) {
        std::vector<hpx::future<void>> futures;

        IntSet theSet;

        for (int threadId = 0; threadId < nThreads; ++threadId) {
            futures.push_back(hpx::async(hpx::launch::async, [threadId, &theSet, &workSize, &nThreads]() {
                for (int i = 0; i < workSize; ++i) {
                    auto token = i * nThreads + threadId;

                    blackBox(theSet.add(token));
                    blackBox(theSet.contains(token));
                    blackBox(theSet.remove(token));
                    blackBox(theSet.contains(token));
                }

                for (int i = 0; i < workSize; i++) {
                    blackBox(theSet.add(i * nThreads + threadId));
                }

                for (int i = 0; i < workSize; i++) {
                    blackBox(theSet.contains(i * nThreads + threadId));
                }

                for (int i = 0; i < workSize; i++) {
                    blackBox(theSet.remove(i * nThreads + threadId));
                }

                for (int i = 0; i < workSize; i++) {
                    blackBox(theSet.contains(i * nThreads + threadId));
                }
            }));
        }

        for (auto&& worker : futures) {
            worker.get();
        }
    }
    #endif

} // parallel_bench::sets

using namespace parallel_bench;
using namespace parallel_bench::sets;

template <template<typename, typename> class Set, MutexType Mutex, typename... Rest>
void benchSets(BenchParameters const& params, std::string const& setName, std::optional<std::string> const& which) {
    auto lockName = locks::LockTraits<Mutex>::name;
    std::string specific(setName);
    specific += "@";
    specific += lockName;

    if (!which || *which == lockName) {
        #if HAVE_HPX
        if(lockName == "hpx::mutex" || lockName == "hpx::spinlock") {
            std::cout << "Bench hpx set: " << specific << std::endl;
            writeBenchResult(params, specific, measure([&params]() {
                benchHpxSet<Set<int, Mutex>>(params.getNThreads(), params.getWorkPerThread());
            }));
        } else
        #endif
        {
            std::cout << "Bench set: " << specific << std::endl;
            writeBenchResult(params, specific, measure([&params]() {
                benchSet<Set<int, Mutex>>(params.getNThreads(), params.getWorkPerThread());
            }));
        }

        params.coolOff();
    }

    if constexpr (sizeof...(Rest) > 0) {
        benchSets<Set, Rest...>(params, setName, which);
    }
}

int main() {
    BenchParameters params("c++", "setByLocks");

    benchSets<FineGrainedSet,
              #if HAVE_HPX
              hpx::mutex,
              hpx::spinlock
              #else
              std::mutex,
              std::recursive_mutex,
              locks::ALock<100>,
              locks::BackoffLock<1, 17>,
              locks::CLHLock,
              locks::IdLock,
              locks::MCSLock,
              locks::OptimizedALock<100>,
              locks::TASLock,
              locks::TIdLock,
              locks::TTASLock,
              locks::TwoCounterLock
              #endif
    >(params, "FineGrainedSet", params.getWhich());

    return 0;
}
