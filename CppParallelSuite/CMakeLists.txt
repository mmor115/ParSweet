cmake_minimum_required(VERSION 3.26)
project(CppParallelSuite)

set(CMAKE_CXX_STANDARD 23)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(TestBarrier Barrier.cpp TestBarrier.cpp)

link_libraries(Threads::Threads)

