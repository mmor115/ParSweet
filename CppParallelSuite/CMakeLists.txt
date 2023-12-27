cmake_minimum_required(VERSION 3.26)
project(CppParallelSuite)

set(CMAKE_CXX_STANDARD 23)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(TestBarrier
        barrier/Barrier.hpp
        bench/TestBarrier.cpp)

add_executable(TestLockHashMap
        lock_hash_map/LockHashMap.hpp
        bench/TestLockHashMap.cpp
        lock_hash_map/Node.hpp
        lock_hash_map/NodeHead.hpp
        KeyType.hpp)

link_libraries(Threads::Threads)

