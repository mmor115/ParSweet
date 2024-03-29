#include "../../sets/FineGrainedSet.hpp"
#include "../../sets/LazySet.hpp"
#include "../../sets/LockHashSet.hpp"
#include "../../sets/OptimisticSet.hpp"
#include <future>
#include <iostream>
#include <mutex>
#include <vector>

#define TEST_X \
    if (!x) { return false; }
#define TEST_NOT_X \
    if (x) { return false; }

namespace parallel_test::sets {
    using namespace parallel_suite;
    using namespace parallel_suite::sets;

    constexpr static int Threads = 12;
    constexpr static int WorkSize = 1000 / Threads;

    template <SetType<int> IntSet>
    bool testA() {
        std::vector<std::future<bool>> futures;

        IntSet theSet;

        for (int threadId = 0; threadId < Threads; ++threadId) {
            futures.push_back(std::async(std::launch::async, [threadId, &theSet]() {
                bool x;

                for (int i = 0; i < WorkSize; ++i) {
                    auto token = i * Threads + threadId;

                    x = theSet.add(token);
                    TEST_X;
                    x = theSet.contains(token);
                    TEST_X;
                    x = theSet.remove(token);
                    TEST_X;
                    x = theSet.contains(token);
                    TEST_NOT_X;
                }

                for (int i = 0; i < WorkSize; i++) {
                    x = theSet.add(i * Threads + threadId);
                    TEST_X;
                }

                for (int i = 0; i < WorkSize; i++) {
                    x = theSet.contains(i * Threads + threadId);
                    TEST_X;
                }

                for (int i = 0; i < WorkSize; i++) {
                    x = theSet.remove(i * Threads + threadId);
                    TEST_X;
                }

                for (int i = 0; i < WorkSize; i++) {
                    x = theSet.contains(i * Threads + threadId);
                    TEST_NOT_X;
                }

                return true;
            }));
        }

        for (auto&& worker : futures) {
            if (!worker.get()) {
                return false;
            }
        }

        return true;
    }

    template <SetType<int>... Sets>
    bool testAs() {
        return (testA<Sets>() && ...);
    }
} // namespace parallel_test::sets

using namespace parallel_test::sets;

int main() {
    bool ok = testAs<
            LockHashSet<int, 16, std::mutex>,
            FineGrainedSet<int, std::mutex>,
            OptimisticSet<int, std::mutex>,
            LazySet<int, std::mutex>>();
    return ok ? 0
              : 0xBAD;
}
