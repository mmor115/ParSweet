#include "Bench.hpp"

#include <vector>
#include <future>

#include "../MutexType.hpp"
#include "../maps/MapTypes.hpp"
#include "../maps/LockHashMap.hpp"
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
#if HAVE_HPX
#include <hpx/hpx.hpp>
#include <hpx/hpx_main.hpp>
#endif

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


namespace parallel_bench::maps {
    using namespace parallel_suite;
    using namespace parallel_suite::maps;

    template <MapType<int, int> IntMap>
    void benchMap(usize const& nThreads, usize const& workSize) {
        std::vector<std::future<void>> futures;

        IntMap theMap;

        for (int threadId = 0; threadId < nThreads; ++threadId) {
            futures.push_back(std::async(std::launch::async, [threadId, &theMap, &nThreads, &workSize]() {
                int out;

                for (int i = 0; i < workSize; ++i) {
                    auto token = i * nThreads + threadId;
                    blackBox(theMap.put(token, token));
                    blackBox(theMap.get(token, out));
                    blackBox(out);
                    blackBox(theMap.del(token));
                    blackBox(theMap.get(token, out));
                    blackBox(out);
                }

                for (int i = 0; i < workSize; i++) {
                    auto token = i * nThreads + threadId;
                    blackBox(theMap.put(token, token));
                }

                for (int i = 0; i < workSize; i++) {
                    auto token = i * nThreads + threadId;
                    blackBox(theMap.get(token, out));
                    blackBox(out);
                }

                for (int i = 0; i < workSize; i++) {
                    auto token = i * nThreads + threadId;
                    blackBox(theMap.del(token));
                }

                for (int i = 0; i < workSize; i++) {
                    auto token = i * nThreads + threadId;
                    blackBox(theMap.get(token, out));
                    blackBox(out);
                }
            }));
        }

        for (auto&& worker : futures) {
            worker.get();
        }
    }
} // parallel_bench::maps

using namespace parallel_bench;
using namespace parallel_bench::maps;

template <template<typename, typename, auto, typename> class Map, usize Buckets, MutexType Mutex, typename... Rest>
void benchMaps(BenchParameters const& params, std::string const& mapName, std::optional<std::string> const& which) {
    auto lockName = locks::LockTraits<Mutex>::name;
    std::string specific(mapName);
    specific += "@";
    specific += lockName;

    if (!which || *which == lockName) {
        writeBenchResult(params, specific, measure([&params]() {
            benchMap<Map<int, int, Buckets, Mutex>>(params.getNThreads(), params.getWorkPerThread());
        }));
        params.coolOff();
    }

    if constexpr (sizeof...(Rest) > 0) {
        benchMaps<Map, Buckets, Rest...>(params, mapName, which);
    }
}


int main() {
    BenchParameters params("c++", "mapByLocks");

    benchMaps<LockHashMap, 16,
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
#if HAVE_HPX
,hpx::mutex
,hpx::spinlock
#endif
    >(params, "LockHashMap", params.getWhich());

    return 0;
}
