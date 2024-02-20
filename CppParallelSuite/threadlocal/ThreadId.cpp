
#include "ThreadId.hpp"

namespace parallel_suite::threadlocal {
    thread_local usize ThreadId::id{0};
    std::atomic<usize> ThreadId::idCounter{1};
} // namespace parallel_suite::threadlocal