// Thread-safety regression guard for the supported usage pattern:
// "one i18n::I18N instance per thread".
//
// Purpose: prove that independent instances are race-free under TSan, so a
// future change that inadvertently shares mutable state between instances
// (e.g., a static/global cache) would be caught.
//
// This test does NOT attempt to make I18N thread-safe on a shared instance.
// The class-level contract in include/i18ncpp.h explicitly forbids that
// pattern — see the \warning docblock above `class I18N`.
//
// Compiled only when the build is configured with -DI18N_ENABLE_TSAN=ON on a
// non-MSVC toolchain (see tests/CMakeLists.txt). Outside that configuration
// this translation unit is an empty TU.

#ifdef I18N_TSAN_BUILD

#include <gtest/gtest.h>
#include "i18ncpp.h"

#include <string>
#include <thread>
#include <vector>

TEST(I18NThreadSafety, IndependentInstancesAreSafe) {
    constexpr int kThreads = 8;
    constexpr int kIters = 10'000;

    std::vector<std::thread> workers;
    workers.reserve(kThreads);

    for (int t = 0; t < kThreads; ++t) {
        workers.emplace_back([] {
            i18n::I18N inst;
            inst.loadLocale("en", std::string(FIXTURES_DIR) + "/en.json");
            inst.setLocale("en");
            for (int i = 0; i < kIters; ++i) {
                auto s = inst.tr("greeting");
                (void)s;
            }
        });
    }

    for (auto& w : workers) {
        w.join();
    }

    SUCCEED();  // TSan will fail the test if it detects a race.
}

#endif  // I18N_TSAN_BUILD
