
#ifndef THREADLOCAL_HPP
#define THREADLOCAL_HPP

#include <pthread.h>
#include "../Types.hpp"
#include "ThreadId.hpp"

namespace parallel_suite::threadlocal {

    template <typename T, usize N = 100 /*This is terrible*/>
    class ThreadLocal {
    private:
        T* dataArr;
        bool* setArr;

        [[nodiscard]]
        inline usize getIdx() const {
            return ThreadId::get() % N;
        }

    public:
        ThreadLocal() requires std::is_default_constructible_v<T> {
            static_assert(N > 0);

            dataArr = new T[N];
            setArr = new bool[N];
            for (usize i = 0; i < N; i++) {
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
} // parallel_suite::threadlocal

#endif //THREADLOCAL_HPP
