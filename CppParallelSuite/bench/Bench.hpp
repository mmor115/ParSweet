
#ifndef BENCH_PARAMETERS_HPP
#define BENCH_PARAMETERS_HPP

#include <string>
#include <utility>
#include <stdexcept>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "../Types.hpp"

namespace parallel_bench {

    class BenchParameters {
    private:
        std::string lang;
        std::string category;
        std::string machine{};
        usize nThreads{};
        usize workPerThread{};

#ifdef NDEBUG
        static constexpr bool debug{false};
#else
        static constexpr bool debug{true};
#endif
    public:
        BenchParameters(std::string lang, std::string category) : lang(std::move(lang)),
                                                                  category(std::move(category)) {
            if (auto* szNThreads = std::getenv("PSWEET_NTHREADS")) {
                nThreads = std::strtoll(szNThreads, nullptr, 10);
            } else {
                throw std::runtime_error("bad PSWEET_NTHREADS");
            }

            if (auto* szWorkPerThread = std::getenv("PSWEET_WORK_PER_THREAD")) {
                workPerThread = std::strtoll(szWorkPerThread, nullptr, 10);
            } else {
                throw std::runtime_error("bad PSWEET_WORK_PER_THREAD");
            }

            if (auto* szMachine = std::getenv("PSWEET_MACHINE")) {
                machine = szMachine;
            } else {
                throw std::runtime_error("bad PSWEET_MACHINE");
            }
        }

        [[nodiscard]]
        inline std::string const& getLang() const {
            return lang;
        }

        [[nodiscard]]
        inline std::string const& getCategory() const {
            return category;
        }

        [[nodiscard]]
        inline std::string const& getMachine() const {
            return machine;
        }

        [[nodiscard]]
        inline usize const& getNThreads() const {
            return nThreads;
        }

        [[nodiscard]]
        inline usize const& getWorkPerThread() const {
            return workPerThread;
        }

        [[nodiscard]]
        inline static consteval bool getDebug() {
            return debug;
        }
    };

    using Duration_t = decltype(std::chrono::high_resolution_clock::now() - std::chrono::high_resolution_clock::now());

    template <typename F, typename... Args>
    inline Duration_t measure(F&& fn, Args&&... args) {
        auto t1 = std::chrono::high_resolution_clock::now();
        fn(std::forward<Args>(args)...);
        auto t2 = std::chrono::high_resolution_clock::now();
        return t2 - t1;
    }

    template <typename D>
    inline auto countDuration(Duration_t const& duration) {
        return std::chrono::duration_cast<D>(duration).count();
    }

    void writeBenchResult(BenchParameters const& params, std::string const& specific, Duration_t const& duration) {
        static constexpr auto outPath{"psweet.csv"};

        std::ofstream f;
        if (!std::filesystem::exists(outPath)) {
            f = std::ofstream(outPath);
            f << "lang,category,specific,machine,nThreads,workPerThread,ms,debug" << std::endl;
        } else {
            f = std::ofstream(outPath, std::ios::app);
        }

        f << params.getLang() << "," << params.getCategory() << "," << specific << "," << params.getMachine() << ","
          << params.getNThreads() << "," << params.getWorkPerThread() << "," << countDuration<std::chrono::milliseconds>(duration)
          << "," << BenchParameters::getDebug() << std::endl;

        std::cout << "Wrote to " << std::filesystem::absolute(std::filesystem::path(outPath)) << std::endl;
    }

} // parallel_bench

#endif //BENCH_PARAMETERS_HPP