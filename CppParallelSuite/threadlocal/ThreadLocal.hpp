
#ifndef THREADLOCAL_HPP
#define THREADLOCAL_HPP

#include <pthread.h>

namespace parallel_suite::threadlocal {

    template <typename T>
    class ThreadLocal {
    private:
        pthread_key_t key;
        T* (*creator)();

        static T* defaultCreator() {
            return new T();
        }

        static void defaultDeleter(T* ptr) {
            delete ptr;
        }

    public:
        ThreadLocal() requires std::is_default_constructible_v<T> : key(), creator(defaultCreator) {
            auto err = pthread_key_create(&key, reinterpret_cast<void (*)(void*)>(defaultDeleter));
            if (err != 0) {
                throw std::system_error(err, std::generic_category(), "pthread_key_create failed.");
            }
        }

        explicit ThreadLocal(void (*deleter)(T*)) requires std::is_default_constructible_v<T> : key(), creator(defaultCreator) {
            auto err = pthread_key_create(&key, reinterpret_cast<void (*)(void*)>(deleter));
            if (err != 0) {
                throw std::system_error(err, std::generic_category(), "pthread_key_create failed.");
            }
        }

        ThreadLocal(T* (*creator)(), void (*deleter)(T*)) : key(), creator(creator) {
            auto err = pthread_key_create(&key, reinterpret_cast<void (*)(void*)>(deleter));
            if (err != 0) {
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

        void init() {
            void* ptr = pthread_getspecific(key);

            if (!ptr) {
                auto err = pthread_setspecific(key, (void*) creator());
                if (err != 0) {
                    throw std::system_error(err, std::generic_category(), "pthread_setspecific failed.");
                }
            }
        }

        void set(T val) {
            void* ptr = pthread_getspecific(key);

            if (ptr) {
                *((T*) ptr) = val;
            } else {
                auto err = pthread_setspecific(key, (void*) creator());
                if (err != 0) {
                    throw std::system_error(err, std::generic_category(), "pthread_setspecific failed.");
                }
                *((T*) pthread_getspecific(key)) = val;
            }
        }

        void setPtr(T* ptr) {
            auto err = pthread_setspecific(key, (void*) ptr);
            if (err != 0) {
                throw std::system_error(err, std::generic_category(), "pthread_setspecific failed.");
            }
        }
    };
} // parallel_suite::threadlocal

#endif //THREADLOCAL_HPP
