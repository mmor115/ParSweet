#ifndef BARRIER_H
#define BARRIER_H

#include <thread>
#include <mutex>
#include <condition_variable>

namespace parallel_suite {

    template <typename Mutex=std::mutex, typename Cond=std::condition_variable>
    class Barrier {
    private:
        int const nThreads;
        Mutex mutex;
        Cond cond;

        int count, march;

    public:
        explicit Barrier(int threads) : nThreads(threads), mutex(), cond(), count(0), march(0) { }

        void sync() {
            std::unique_lock lock(mutex);

            ++count;
            int check = march;

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

} // parallel_suite

#endif //BARRIER_H
