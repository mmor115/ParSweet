
#include "Bench.hpp"

#include "../MutexType.hpp"
#include "../locks/BackoffLock.hpp"
#include "BlackBox.hpp"
#include "UnrolledFor.hpp"
#include <cassert>
#include <optional>
#include <thread>
#include <vector>

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
} // namespace parallel_bench::locks

using namespace parallel_bench;
using namespace parallel_bench::locks;

int main() {
    BenchParameters params("c++", "tune BackoffLock");

    std::optional<std::tuple<std::string, Duration_t>> best(std::nullopt);

    unrolledFor<1, 10>([&params, &best]<typename I>(I i) {
        unrolledFor<I::value, 20>([&i = I::value, &params, &best](auto j) {
            std::string specific("BackoffLock");
            formatSpecific(specific, i, j);

            auto d = measure([&params, &j]() {
                benchLock<BackoffLock<I::value, j>>(params.getNThreads(), params.getWorkPerThread());
            });

            writeBenchResult(params, specific, d);
            params.coolOff();

            if (!best || d < std::get<Duration_t>(*best)) {
                best = std::make_tuple(std::move(specific), std::move(d));
            }
        });
    });

    assert(best);
    std::cout << "Best: " << std::get<std::string>(*best) << std::endl;

    return 0;
}
