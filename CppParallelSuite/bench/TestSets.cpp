#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <future>
#include "../sets/FineGrainedSet.hpp"
#include "../locks/TTASLock.hpp"

#define TEST_X if (!x) { return false; }
#define TEST_NOT_X if (x) { return false; }

namespace parallel_bench::sets {
    using namespace parallel_suite;
    using namespace parallel_suite::sets;

    constexpr static int Threads = 12;
    constexpr static int WorkSize = 1000;
    constexpr static int WorkRange = 15;

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
} // parallel_bench::sets

using namespace parallel_bench::sets;

int main() {
    bool ok = testA<FineGrainedSet<int, parallel_suite::locks::TTASLock>>();
    return ok ? 0
              : 0xBAD;
}
