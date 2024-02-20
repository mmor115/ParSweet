#ifndef BARRIER_HPP
#define BARRIER_HPP

#include "../MutexType.hpp"
#include <condition_variable>
#include <mutex>
#include <thread>

namespace parallel_suite::barrier {

    template <MutexType Mutex = std::mutex, typename Cond = std::condition_variable>
    class Barrier {
    private:
        int const nThreads;
        Mutex mutex;
        Cond cond;

        int count, march;

    public:
        explicit Barrier(int threads) : nThreads(threads), mutex(), cond(), count(0), march(0) {}

        void sync() {
            std::unique_lock lock(mutex);

            ++count;
            int const check = march;

            if (count == nThreads) {
                count = 0;
                ++march;
                lock.unlock();
                cond.notify_all();
            } else {
                cond.wait(lock, [&] { return check != march; });
            }
        }
    };

} // namespace parallel_suite::barrier

#endif //BARRIER_HPP
