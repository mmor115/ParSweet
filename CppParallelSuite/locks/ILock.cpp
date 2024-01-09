#include "ILock.hpp"

namespace parallel_suite::locks {
    std::atomic<usize> ILock::ticketCounter{1};
}
