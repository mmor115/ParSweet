#include <iostream>
#include <vector>
#include "../../barrier/Barrier.hpp"

namespace parallel_test::barrier {
    using namespace parallel_suite::barrier;

    constexpr int N = 12;
    constexpr int T = 10;

    Barrier barrier(N);

    void work(int threadId) {
        for (int t = 0; t < T; ++t) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100 + std::rand() % 400));
            std::cout << "March #" << t << " by thread " << threadId << std::endl;
            barrier.sync();
        }
    }
} // parallel_test::barrier

using namespace parallel_test::barrier;

int main() {
    std::vector<std::thread> workers;

    for (int i = 0; i < N; ++i) {
        workers.emplace_back(work, i);
    }

    for (auto&& worker : workers) {
        worker.join();
    }

    return 0;
}
