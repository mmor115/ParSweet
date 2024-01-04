
#ifndef THREADLOCAL_HPP
#define THREADLOCAL_HPP

#include <pthread.h>
#include <cstdlib>
#include <format>

namespace parallel_suite::threadlocal {

    template <typename T>
    class ThreadLocal {
    private:
        pthread_key_t key;
    public:
        ThreadLocal() : key{} {
            int err;
            if ((err = pthread_key_create(&key, free)) != 0) {
                throw std::system_error(err, std::generic_category(), std::format("pthread_key_create returned error {}.", err));
            }
        }

        ThreadLocal(ThreadLocal&) = delete;

        ~ThreadLocal() {
            pthread_key_delete(key);
        }

        T get() const {
            void* ptr = pthread_getspecific(key);

            if (ptr) {
                return *((T*) ptr);
            } else {
                throw std::logic_error("ThreadLocal value has not been set.");
            }
        }

        void set(T val) {
            void* ptr = pthread_getspecific(key);

            if (ptr) {
                *((T*) ptr) = val;
            } else {
                int err;
                if ((err = pthread_setspecific(key, malloc(sizeof(T)))) != 0) {
                    throw std::system_error(err, std::generic_category(), std::format("pthread_setspecific returned error {}.", err));
                }
                *((T*) pthread_getspecific(key)) = val;
            }
        }
    };
} // parallel_suite::threadlocal

#endif //THREADLOCAL_HPP
