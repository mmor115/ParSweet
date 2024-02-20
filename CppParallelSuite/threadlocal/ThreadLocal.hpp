
#ifndef THREAD_LOCAL_HPP
#define THREAD_LOCAL_HPP

#include "../Types.hpp"
#include "ThreadId.hpp"
#include <pthread.h>

namespace parallel_suite::threadlocal {

    template <typename T, usize MaxThreads = N_THREADS_ALLOC>
    class ThreadLocal {
    private:
        T* dataArr;
        bool* setArr;

        [[nodiscard]]
        inline usize getIdx() const {
            return ThreadId::get() % MaxThreads;
        }

    public:
        ThreadLocal()
        requires std::is_default_constructible_v<T>
        {
            static_assert(MaxThreads > 0);

            dataArr = new T[MaxThreads];
            setArr = new bool[MaxThreads];
            for (usize i = 0; i < MaxThreads; i++) {
                setArr[i] = false;
            }
        }

        ThreadLocal(ThreadLocal&) = delete;

        ~ThreadLocal() {
            delete[] dataArr;
            delete[] setArr;
        }

        [[nodiscard]]
        T& get() const {
            auto idx = getIdx();
            if (!setArr[idx]) {
                dataArr[idx] = T();
                setArr[idx] = true;
            }
            return dataArr[idx];
        }

        [[nodiscard]]
        bool isSet() const {
            auto idx = getIdx();
            return setArr[idx];
        }

        void set(T val) {
            auto idx = getIdx();
            setArr[idx] = true;
            dataArr[idx] = val;
        }
    };
} // namespace parallel_suite::threadlocal

#endif //THREAD_LOCAL_HPP
