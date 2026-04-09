#include "bench_harness.h"
#include "i18ncpp.h"
#include <string>
#include <ctime>

static std::string fixturesDir() {
    return FIXTURES_DIR;
}

int main() {
    const size_t ITERATIONS = 100000;
    std::vector<bench::BenchResult> results;

    // Setup
    i18n::I18N i18n;
    i18n.loadLocale("en", (fixturesDir() + "/en.json").c_str());
    i18n.setLocale("en");
    i18n.setFallbackLocale("en");

    // Fixed time for deterministic date benchmarks
    std::tm fixed_time = {};
    fixed_time.tm_year = 126; // 2026
    fixed_time.tm_mon = 3;    // April
    fixed_time.tm_mday = 9;
    fixed_time.tm_hour = 14;
    fixed_time.tm_min = 30;
    fixed_time.tm_sec = 0;
    fixed_time.tm_wday = 4;   // Thursday

    // BM_FormatNumber: number formatting
    results.push_back(bench::run_benchmark("FormatNumber", ITERATIONS, [&]() {
        i18n.formatNumber(123456.78);
    }));

    // BM_FormatNumberCached: second call (cache hit)
    i18n.formatNumber(999.99); // prime the cache
    results.push_back(bench::run_benchmark("FormatNumberCached", ITERATIONS, [&]() {
        volatile auto r = i18n.formatNumber(999.99);
    }));

    // BM_FormatPrice: currency formatting
    results.push_back(bench::run_benchmark("FormatPrice", ITERATIONS, [&]() {
        i18n.formatPrice(99.99);
    }));

    // BM_FormatPriceCached: cached price
    i18n.formatPrice(49.95); // prime
    results.push_back(bench::run_benchmark("FormatPriceCached", ITERATIONS, [&]() {
        volatile auto r = i18n.formatPrice(49.95);
    }));

    // BM_FormatDate: date formatting with fixed time
    results.push_back(bench::run_benchmark("FormatDate", ITERATIONS, [&]() {
        volatile auto r = i18n.formatDate("short_date", &fixed_time);
    }));

    // BM_FormatDateCached: cached date
    i18n.formatDate("short_time", &fixed_time); // prime
    results.push_back(bench::run_benchmark("FormatDateCached", ITERATIONS, [&]() {
        volatile auto r = i18n.formatDate("short_time", &fixed_time);
    }));

    std::cout << "\n=== Formatting Benchmarks ===\n\n";
    bench::print_results(results);

    return 0;
}
