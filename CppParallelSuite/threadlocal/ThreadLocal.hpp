
#ifndef THREADLOCAL_HPP
#define THREADLOCAL_HPP

#include <pthread.h>
#include <cstdlib>

namespace parallel_suite::threadlocal {

    template <typename T>
    class ThreadLocal {
    private:
        pthread_key_t key;
    public:
        ThreadLocal() : key{} {
            int err;
            if ((err = pthread_key_create(&key, free)) != 0) {
                throw std::system_error(err, std::generic_category(), "pthread_key_create failed.");
            }
        }

        ThreadLocal(ThreadLocal&) = delete;

        ~ThreadLocal() {
            pthread_key_delete(key);
        }

        [[nodiscard]]
        T get() const {
            void* ptr = pthread_getspecific(key);

            if (ptr) {
                return *((T*) ptr);
            } else {
                throw std::logic_error("ThreadLocal value has not been set.");
            }
        }

        [[nodiscard]]
        T* getPtr() const {
            return (T*) pthread_getspecific(key);
        }

        [[nodiscard]]
        bool isSet() const {
            void* ptr = pthread_getspecific(key);
            return ptr != nullptr;
        }

        void allocate() {
            void* ptr = pthread_getspecific(key);

            if (!ptr) {
                int err;
                if ((err = pthread_setspecific(key, malloc(sizeof(T)))) != 0) {
                    throw std::system_error(err, std::generic_category(), "pthread_setspecific failed.");
                }
            }
        }

        void set(T val) {
            void* ptr = pthread_getspecific(key);

            if (ptr) {
                *((T*) ptr) = val;
            } else {
                int err;
                if ((err = pthread_setspecific(key, malloc(sizeof(T)))) != 0) {
                    throw std::system_error(err, std::generic_category(), "pthread_setspecific failed.");
                }
                *((T*) pthread_getspecific(key)) = val;
            }
        }

        void setPtr(T* ptr) {
            int err;
            if ((err = pthread_setspecific(key, (void*) ptr)) != 0) {
                throw std::system_error(err, std::generic_category(), "pthread_setspecific failed.");
            }
        }
    };
} // parallel_suite::threadlocal

#endif //THREADLOCAL_HPP
