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
    message(STATUS "UBSan enabled")
endif()

find_package(Threads REQUIRED)

add_executable(TestBarrier
        barrier/Barrier.hpp
        bench/test/TestBarrier.cpp)

add_executable(TestLocks
        bench/test/TestLocks.cpp
        MutexType.hpp
        locks/TASLock.hpp
        locks/TTASLock.hpp
        locks/ALock.hpp
        threadlocal/ThreadLocal.hpp
        locks/BackoffLock.hpp
        Rand.hpp
        locks/CLHLock.hpp
        locks/MCSLock.hpp
        threadlocal/ThreadLocalRand.cpp
        threadlocal/ThreadId.cpp
        locks/IdLock.hpp
        locks/TIdLock.hpp
        locks/TwoCounterLock.hpp)

add_executable(TestSets
        bench/test/TestSets.cpp
        sets/FineGrainedSet.hpp
        sets/OptimisticSet.hpp
        sets/LazySet.hpp
        sets/NodeMarkers.hpp
        sets/SetNode.hpp
        sets/SetTypes.hpp)

add_executable(TestMaps
        bench/test/TestMaps.cpp
        maps/LockHashMap.hpp
        maps/MapTypes.hpp)

link_libraries(Threads::Threads)

enable_testing()

add_test(NAME TestLocks
         COMMAND ./TestLocks)
add_test(NAME TestBarrier
         COMMAND ./TestBarrier)
add_test(NAME TestSets
         COMMAND ./TestSets)
add_test(NAME TestMaps
         COMMAND ./TestMaps)
