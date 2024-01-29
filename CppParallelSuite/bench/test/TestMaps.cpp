#include <iostream>
#include <vector>
#include <future>

#include "../../maps/MapTypes.hpp"
#include "../../maps/LockHashMap.hpp"

#define TEST_X if (!x) { return false; }
#define TEST_NOT_X if (x) { return false; }

namespace parallel_test::maps {
    using namespace parallel_suite;
    using namespace parallel_suite::maps;

    constexpr static int Threads = 12;
    constexpr static int WorkSize = 1000 / Threads;

    template <MapType<int, int> IntMap>
    bool testA() {
        std::vector<std::future<bool>> futures;

        IntMap theMap;

        for (int threadId = 0; threadId < Threads; ++threadId) {
            futures.push_back(std::async(std::launch::async, [threadId, &theMap]() {
                bool x;
                int out;

                for (int i = 0; i < WorkSize; ++i) {
                    auto token = i * Threads + threadId;

                    x = theMap.put(token, token);
                    TEST_X;
                    x = theMap.get(token, out);
                    TEST_X;
                    x = token == out;
                    TEST_X;

                    theMap.del(token);
                    x = theMap.get(token, out);
                    TEST_NOT_X;
                }

                for (int i = 0; i < WorkSize; i++) {
                    auto token = i * Threads + threadId;
                    x = theMap.put(token, token);
                    TEST_X;
                }

                for (int i = 0; i < WorkSize; i++) {
                    auto token = i * Threads + threadId;
                    x = theMap.get(token, out);
                    TEST_X;
                    x = token == out;
                    TEST_X;
                }

                for (int i = 0; i < WorkSize; i++) {
                    auto token = i * Threads + threadId;
                    x = theMap.del(token);
                    TEST_X;
                }

                for (int i = 0; i < WorkSize; i++) {
                    auto token = i * Threads + threadId;
                    x = theMap.get(token, out);
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

    template <MapType<int, int>... Maps>
    bool testAs() {
        return (testA<Maps>() && ...);
    }
} // parallel_test::sets

using namespace parallel_test::maps;

int main() {
    bool ok = testAs<
            LockHashMap<int, int, 16>
    >();
    return ok ? 0
              : 0xBAD;
}
