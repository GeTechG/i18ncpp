#include "bench_harness.h"
#include "i18ncpp.h"
#include <string>

static std::string fixturesDir() {
    return FIXTURES_DIR;
}

int main() {
    const size_t ITERATIONS = 100000;
    std::vector<bench::BenchResult> results;

    // Setup: load locales
    i18n::I18N i18n;
    i18n.loadLocale("en", (fixturesDir() + "/en.json").c_str());
    i18n.loadLocale("fr", (fixturesDir() + "/fr.json").c_str());
    i18n.loadLocale("ru", (fixturesDir() + "/ru.json").c_str());
    i18n.setLocale("en");
    i18n.setFallbackLocale("en");

    // BM_TrSimpleKey: simple flat lookup
    results.push_back(bench::run_benchmark("TrSimpleKey", ITERATIONS, [&]() {
        volatile auto r = i18n.tr("greeting");
    }));

    // BM_TrNestedKey: dot-notation key (menu.file.open)
    results.push_back(bench::run_benchmark("TrNestedKey", ITERATIONS, [&]() {
        volatile auto r = i18n.tr("menu.file.open");
    }));

    // BM_TrDeepNested: deep dot-notation key
    results.push_back(bench::run_benchmark("TrDeepNested", ITERATIONS, [&]() {
        volatile auto r = i18n.tr("deep.nested.key");
    }));

    // BM_TrMissingFallback: key only in en, accessed from ru locale
    {
        i18n::I18N i18n_fb;
        i18n_fb.loadLocale("en", (fixturesDir() + "/en.json").c_str());
        i18n_fb.loadLocale("ru", (fixturesDir() + "/ru.json").c_str());
        i18n_fb.setLocale("ru");
        i18n_fb.setFallbackLocale("en");
        results.push_back(bench::run_benchmark("TrMissingFallback", ITERATIONS, [&]() {
            volatile auto r = i18n_fb.tr("only_in_en");
        }));
    }

    // BM_TrPlural: plural form selection
    results.push_back(bench::run_benchmark("TrPlural", ITERATIONS, [&]() {
        volatile auto r = i18n.trPlural("items_plural", 5);
    }));

    // BM_KeyExists: existence check
    results.push_back(bench::run_benchmark("KeyExists", ITERATIONS, [&]() {
        volatile auto r = i18n.keyExists("greeting");
    }));

    // BM_KeyExistsMissing: existence check for missing key
    results.push_back(bench::run_benchmark("KeyExistsMissing", ITERATIONS, [&]() {
        volatile auto r = i18n.keyExists("nonexistent.key");
    }));

    std::cout << "\n=== Lookup Benchmarks ===\n\n";
    bench::print_results(results);

    return 0;
}
