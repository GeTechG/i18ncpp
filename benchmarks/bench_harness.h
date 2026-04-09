#pragma once

#include <chrono>
#include <functional>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

namespace bench {

struct BenchResult {
    std::string name;
    double ns_per_op;
    size_t iterations;
};

inline BenchResult run_benchmark(const std::string& name, size_t iterations, std::function<void()> fn) {
    // Warm-up: 10% of iterations, discarded
    size_t warmup = iterations / 10;
    for (size_t i = 0; i < warmup; ++i) {
        fn();
    }

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; ++i) {
        fn();
    }
    auto end = std::chrono::high_resolution_clock::now();

    double total_ns = static_cast<double>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
    );
    double ns_per_op = total_ns / static_cast<double>(iterations);

    return {name, ns_per_op, iterations};
}

inline void print_results(const std::vector<BenchResult>& results) {
    // Find longest name for alignment
    size_t max_name = 0;
    for (const auto& r : results) {
        max_name = std::max(max_name, r.name.size());
    }

    std::cout << std::string(max_name + 35, '-') << "\n";
    std::cout << std::left << std::setw(static_cast<int>(max_name + 2)) << "Benchmark"
              << std::right << std::setw(15) << "ns/op"
              << std::setw(15) << "iterations" << "\n";
    std::cout << std::string(max_name + 35, '-') << "\n";

    for (const auto& r : results) {
        std::cout << std::left << std::setw(static_cast<int>(max_name + 2)) << r.name
                  << std::right << std::setw(15) << std::fixed << std::setprecision(1) << r.ns_per_op
                  << std::setw(15) << r.iterations << "\n";
    }

    std::cout << std::string(max_name + 35, '-') << "\n";
}

} // namespace bench
