
#ifndef THREADLOCAL_HPP
#define THREADLOCAL_HPP

#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

namespace parallel_suite::threadlocal {

    const int base_tid = gettid();
    // This is terrible
    const std::size_t N = 100;

    template <typename T>
    class ThreadLocal {
    private:
        T* data;
        bool *is_set;

    public:
        ThreadLocal() requires std::is_default_constructible_v<T> {
            if(N == 0)
                throw std::logic_error("ThreadLocal is size 0.");
            data = new T[N];
            is_set = new bool[N];
            for(std::size_t i=0;i<N;i++)
                is_set[i] = false;
        }

        ThreadLocal(ThreadLocal&) = delete;

        ~ThreadLocal() {
            delete[] data;
            delete[] is_set;
        }

        [[nodiscard]]
        T& get() const {
            int n = gettid()%N;
            if(is_set[n]) {
                return data[n];
            } else {
                throw std::logic_error("ThreadLocal value has not been set.");
            }
        }

        [[nodiscard]]
        bool isSet() const {
            int n = gettid()%N;
            return is_set[n];
        }

        void set(T val) {
            int n = gettid()%N;
            is_set[n] = true;
            data[n] = val;
        }
    };
} // parallel_suite::threadlocal

#endif //THREADLOCAL_HPP
