
#ifndef THREADLOCAL_HPP
#define THREADLOCAL_HPP

#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

namespace parallel_suite::threadlocal {

    int base_tid = gettid();
    constexpr std::size_t N = 50;

    template <typename T>
    class ThreadLocal {
    private:
        T* data;
        bool *is_set;

    public:
        ThreadLocal() requires std::is_default_constructible_v<T> {
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
            int n = gettid() - base_tid;
            if (n < N) {
                if(is_set[n]) {
                    return data[n];
                } else {
                    throw std::logic_error("ThreadLocal value has not been set.");
                }
            } else {
                throw std::logic_error("ThreadLocal index out of range.");
            }
        }

        [[nodiscard]]
        bool isSet() const {
            int n = gettid() - base_tid;
            if (n < N) {
                return is_set[n];
            } else {
                throw std::logic_error("ThreadLocal index out of range.");
            }
        }

        void init() {
            int n = gettid() - base_tid;
            if (n < N) {
                is_set[n] = false;
            } else {
                throw std::logic_error("ThreadLocal index out of range.");
            }
        }

        void set(T val) {
            int n = gettid() - base_tid;
            if (n < N) {
                is_set[n] = true;
                data[n] = val;
            } else {
                throw std::logic_error("ThreadLocal index out of range.");
            }
        }
    };
} // parallel_suite::threadlocal

#endif //THREADLOCAL_HPP
