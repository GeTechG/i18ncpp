#include "bench_harness.h"
#include "i18ncpp.h"
#include <string>

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

    // BM_InterpolatePositional: single positional param {0}
    results.push_back(bench::run_benchmark("InterpolatePositional", ITERATIONS, [&]() {
        volatile auto r = i18n.tr("welcome", {"Alice"});
    }));

    // BM_InterpolateTwoParams: two positional params {0}, {1}
    results.push_back(bench::run_benchmark("InterpolateTwoParams", ITERATIONS, [&]() {
        volatile auto r = i18n.tr("welcome_pos", {"Alice", "Wonderland"});
    }));

    // BM_InterpolateNamed: named params %{key}
    results.push_back(bench::run_benchmark("InterpolateNamed", ITERATIONS, [&]() {
        nlohmann::json params;
        params["name"] = "Alice";
        volatile auto r = i18n.translate("welcome_named", params);
    }));

    // BM_InterpolatePluralWithParam: trPlural with extra params
    results.push_back(bench::run_benchmark("PluralWithParam", ITERATIONS, [&]() {
        volatile auto r = i18n.trPlural("items_in_cart", 3, {"cart"});
    }));

    // BM_NoPlaceholders: translation with no interpolation
    results.push_back(bench::run_benchmark("NoPlaceholders", ITERATIONS, [&]() {
        volatile auto r = i18n.tr("no_placeholders");
    }));

    std::cout << "\n=== Interpolation Benchmarks ===\n\n";
    bench::print_results(results);

    return 0;
}
