cmake_minimum_required(VERSION 3.22)

find_package(Java REQUIRED)
include(UseJava)

enable_testing()

project (ParallelSuite NONE)

set(CMAKE_JAVA_COMPILE_FLAGS "-source" "1.10" "-target" "1.10")

add_jar(ParallelSuite 
    ./src/edu/lsu/cct/parallelsuite/Barrier.java 
    ./src/edu/lsu/cct/parallelsuite/bench/Misc.java 
    ./src/edu/lsu/cct/parallelsuite/bench/TestToySoldiers.java 
    ./src/edu/lsu/cct/parallelsuite/bench/TestBarrier.java
    ./src/edu/lsu/cct/parallelsuite/bench/TestSets.java
    ./src/edu/lsu/cct/parallelsuite/maps/LockHashMap.java
    ./src/edu/lsu/cct/parallelsuite/maps/SlimMap.java
    ./src/edu/lsu/cct/parallelsuite/ToySoldiersSim.java
    ./src/edu/lsu/cct/parallelsuite/sets/FindCallback.java
    ./src/edu/lsu/cct/parallelsuite/sets/FineGrainedSet.java
    ./src/edu/lsu/cct/parallelsuite/sets/LazySet.java
    ./src/edu/lsu/cct/parallelsuite/sets/OptimisticSet.java
    ./src/edu/lsu/cct/parallelsuite/sets/SlimSet.java)
    #./src/edu/lsu/cct/parallelsuite/maps/LockTreeMap.java

get_target_property(_jarFile ParallelSuite JAR_FILE)
get_target_property(_classDir ParallelSuite CLASSDIR)

message(STATUS "Jar file ${_jarFile}")
message(STATUS "Class compiled to ${_classDir}")

add_test(NAME TestParallelJavaToySoldiers COMMAND ${Java_JAVA_EXECUTABLE} -ea -cp ${_jarFile} edu.lsu.cct.parallelsuite.bench.TestToySoldiers)
add_test(NAME TestParallelJavaBarrier COMMAND ${Java_JAVA_EXECUTABLE} -ea -cp ${_jarFile} edu.lsu.cct.parallelsuite.bench.TestBarrier)
add_test(NAME TestParallelJavaSets COMMAND ${Java_JAVA_EXECUTABLE} -ea -cp ${_jarFile} edu.lsu.cct.parallelsuite.bench.TestSets)
