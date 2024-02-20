
#ifndef THREAD_LOCAL_RAND_HPP
#define THREAD_LOCAL_RAND_HPP

#include "../Rand.hpp"

namespace parallel_suite::threadlocal {
    struct ThreadLocalRand {
        static thread_local IntRand intRand;

        ThreadLocalRand() = delete;
    };
} // namespace parallel_suite::threadlocal

#endif //THREAD_LOCAL_RAND_HPP
