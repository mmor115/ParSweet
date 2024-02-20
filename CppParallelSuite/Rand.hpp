
#ifndef RAND_HPP
#define RAND_HPP

#include <random>

namespace parallel_suite {

    template <typename Dist, typename Num>
    class Rand {
    private:
        std::random_device device;
        std::mt19937 mt;

    public:
        Rand() : device(), mt(device()) {}

        template <typename T1, typename T2>
        requires std::convertible_to<T1, Num> && std::convertible_to<T2, Num>
        Num getRand(T1 origin, T2 bound) {
            Dist dist(origin, bound);
            return dist(mt);
        }
    };

    using IntRand = Rand<std::uniform_int_distribution<int>, int>;
    using DoubleRand = Rand<std::uniform_real_distribution<double>, double>;
} // namespace parallel_suite

#endif // RAND_HPP
