
#ifndef MUTEXTYPE_HPP
#define MUTEXTYPE_HPP

#include <concepts>

namespace parallel_suite {
    template <typename L>
    concept MutexType = std::is_default_constructible_v<L> && requires(L l) {
        l.lock();
        l.unlock();
    };

    template <typename L>
    concept TryLockMutexType = MutexType<L> && requires(L l) {
        { l.try_lock() } -> std::convertible_to<bool>;
    };
} // namespace parallel_suite

#endif //MUTEXTYPE_HPP
