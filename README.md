# ParSweet: A Suite of Codes for Benchmarking and Testing Mutex-Based Parallel Systems

## What?
ParSweet is a collection of small- to medium-sized codes designed to cover common (or uncommon) use cases of mutual
exclusion in parallel programs. It also comes with correctness tests and performance benchmarks.

Coverage currently exists for the following languages/frameworks:
* C++
* HPX
* Java

## How? (C++)
Compile with CMake. GCC >= 12 is needed. Other compilers should work, but have not been thoroughly tested.
HPX is an optional dependency; if it is found, benchmarks that require it will be enabled.

Benchmarks expect the following environment variables at runtime:
* PSWEET_NTHREADS - Number of threads to use.
* PSWEET_WORK_PER_THREAD - How much work each thread should perform. What exactly this entails depends on the specific benchmark.
* PSWEET_MACHINE - A unique identifier. This will be written to the benchmark results file to help identify which machine each result comes from.
* PSWEET_COOLDOWN (Optional) - How long, in milliseconds, to sleep between individual tests within one benchmark program. 
Weaker machines may need this to avoid benchmark results being confounded by thermal throttling. If absent, defaults to 0 (no sleep occurs).

## How? (Java)
TODO

## Who?
See AUTHORS.

## Contributing
TODO