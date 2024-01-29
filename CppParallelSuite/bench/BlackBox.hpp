/*
 * This file (i.e. the blackBox() function) is adapted from code in the Folly library by Facebook.
 * https://github.com/facebook/folly/blob/a3c67e006de0e32ecad1bc406ab74ca767e4caee/folly/Benchmark.h#L243
 *
 * By passing something to blackBox(), we tell the compiler not to optimize it away. We do this to avoid overly
 * optimistic benchmarks.
 */
#ifndef BLACK_BOX_HPP
#define BLACK_BOX_HPP

namespace parallel_bench {
#ifdef _MSC_VER /* MSVC */
#pragma optimize("", off)
    template <class T>
    void blackBox(T&& datum) {
      datum = datum;
    }
#pragma optimize("", on)
#elif defined(__clang__) /* Clang */
    template <class T>
    __attribute__((__optnone__)) void blackBox(T&& datum) {
    }
#else /* GCC */
    template <class T>
    void blackBox(T&& datum) {
        asm volatile("" : "+r" (datum));
    }
#endif
}

#endif //BLACK_BOX_HPP
