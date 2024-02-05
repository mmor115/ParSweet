#include "Bench.hpp"

#include <vector>
#include <future>

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

template <template<typename, typename> class Set, MutexType Mutex, typename... Rest>
void benchSets(BenchParameters const& params, std::string const& setName) {
    std::string specific(setName);
    specific += "@";
    specific += locks::LockTraits<Mutex>::name;

    writeBenchResult(params, specific, measure([&params]() {
        benchSet<Set<int, Mutex>>(params.getNThreads(), params.getWorkPerThread());
    }));

    params.coolOff();

    if constexpr (sizeof...(Rest) > 0) {
        benchSets<Set, Rest...>(params, setName);
    }
}

int main() {
    BenchParameters params("c++", "setByLocks");

    benchSets<FineGrainedSet,
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
    >(params, "FineGrainedSet");

    return 0;
}
