#include "Bench.hpp"

#include <vector>
#include <future>

#include "../sets/FineGrainedSet.hpp"
#include "../sets/OptimisticSet.hpp"
#include "../sets/LazySet.hpp"
#include "BlackBox.hpp"

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

} // parallel_bench::sets

using namespace parallel_bench;
using namespace parallel_bench::sets;

int main() {
    BenchParameters params("c++", "sets");

    writeBenchResult(params, "FineGrainedSet", measure([&params]() {
        benchSet<FineGrainedSet<int>>(params.getNThreads(), params.getWorkPerThread());
    }));

    writeBenchResult(params, "OptimisticSet", measure([&params]() {
        benchSet<OptimisticSet<int>>(params.getNThreads(), params.getWorkPerThread());
    }));

    writeBenchResult(params, "LazySet", measure([&params]() {
        benchSet<LazySet<int>>(params.getNThreads(), params.getWorkPerThread());
    }));

    return 0;
}
