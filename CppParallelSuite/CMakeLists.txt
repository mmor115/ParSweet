cmake_minimum_required(VERSION 3.22)
project(CppParallelSuite)

set(CMAKE_CXX_STANDARD 20)

set(THREADS_PREFER_PTHREAD_FLAG ON)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread")

if (DEFINED ENV{ASAN})
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
    message(STATUS "ASan enabled")
endif()

if (DEFINED ENV{UBSAN})
    add_compile_options(-fsanitize=undefined)
    add_link_options(-fsanitize=undefined)
    message(STATUS "UBan enabled")
endif()

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

add_executable(TestLockTreeMap
        bench/TestLockTreeMap.cpp
        KeyType.hpp
        lock_tree_map/LockTreeMap.hpp
        lock_tree_map/Node.hpp)

add_executable(TestLocks
        bench/TestLocks.cpp
        MutexType.hpp
        locks/TASLock.hpp
        locks/TTASLock.hpp
        locks/ILock.cpp
        locks/ALock.hpp
        threadlocal/ThreadLocal.hpp
        locks/BackoffLock.hpp
        Rand.hpp
        locks/CLHLock.hpp
        locks/MCSLock.hpp
        threadlocal/ThreadLocalRand.cpp)

add_executable(TestSets
        bench/TestSets.cpp
        sets/FineGrainedSet.hpp
        sets/NodeMarkers.hpp
        sets/SetNode.hpp
        sets/SetTypes.hpp)

link_libraries(Threads::Threads)

enable_testing()

add_test(NAME TestLockTreeMap
         COMMAND ./TestLockTreeMap)
add_test(NAME TestLockHashMap
         COMMAND ./TestLockHashMap)
add_test(NAME TestLocks
         COMMAND ./TestLocks)
add_test(NAME TestBarrier
         COMMAND ./TestBarrier)
add_test(NAME TestSets
         COMMAND ./TestSets)
