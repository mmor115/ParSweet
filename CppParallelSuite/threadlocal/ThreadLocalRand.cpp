
#include "ThreadLocalRand.hpp"

namespace parallel_suite::threadlocal {
    thread_local IntRand ThreadLocalRand::intRand{};
} // parallel_suite::threadlocal