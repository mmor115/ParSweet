
#ifndef LOCK_TRAITS_HPP
#define LOCK_TRAITS_HPP

#include "../MutexType.hpp"

namespace parallel_suite::locks {
    template <MutexType L>
    struct LockTraits {
        constexpr static char const* name = nullptr;
    };
} // parallel_suite::locks

#endif //LOCK_TRAITS_HPP
