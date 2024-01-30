#include "Bench.hpp"

#include <vector>
#include <future>

#include "../maps/MapTypes.hpp"
#include "../maps/LockHashMap.hpp"
#include "../maps/SetBasedMap.hpp"
#include "../sets/FineGrainedSet.hpp"
#include "../sets/OptimisticSet.hpp"
#include "../sets/LazySet.hpp"

#include "BlackBox.hpp"

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

int main() {
    BenchParameters params("c++", "maps");

    writeBenchResult(params, "LockHashMap", measure([&params]() {
        benchMap<LockHashMap<int, int>>(params.getNThreads(), params.getWorkPerThread());
    }));

    writeBenchResult(params, "SetBasedMap<FineGrainedSet>", measure([&params]() {
        benchMap<SetBasedMap<int, int, sets::FineGrainedSet>>(params.getNThreads(), params.getWorkPerThread());
    }));

    writeBenchResult(params, "SetBasedMap<OptimisticSet>", measure([&params]() {
        benchMap<SetBasedMap<int, int, sets::OptimisticSet>>(params.getNThreads(), params.getWorkPerThread());
    }));

    writeBenchResult(params, "SetBasedMap<LazySet>", measure([&params]() {
        benchMap<SetBasedMap<int, int, sets::LazySet>>(params.getNThreads(), params.getWorkPerThread());
    }));

    return 0;
}
