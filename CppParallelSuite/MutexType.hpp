
#ifndef MUTEXTYPE_HPP
#define MUTEXTYPE_HPP

#include <concepts>

namespace parallel_suite {
    template <typename L>
    concept MutexType = requires(L l) {
        { L() } -> std::same_as<L>;
        l.lock();
        l.unlock();
    };

    template <typename L>
    concept TryLockMutexType = MutexType<L> && requires(L l) {
        { l.try_lock() } -> std::convertible_to<bool>;
    };
}

#endif //MUTEXTYPE_HPP
