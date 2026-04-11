# CLAUDE.md — i18ncpp agent onboarding

Minimal context for AI agents working on this repo. Keep it current; do not duplicate `.paul/` state.

## Project summary

i18ncpp is a small, dependency-light C++20 internationalization library — translation, pluralization, and locale-aware number/currency/date formatting. Current version: **0.4.0** (stable). It is a C++ port of the [smiti18n](https://github.com/Oval-Tutu/smiti18n) Lua library. The design goal is ICU-free i18n for projects that cannot take on ICU's footprint (e.g. game plugins, embedded tooling).

## Build

CMake + Conan. Only external runtime dep is `nlohmann/json` (vendored under `thirdparty/json`).

```bash
# Conan-driven build (preferred)
conan install . --build=missing
cmake --preset conan-default
cmake --build --preset conan-release

# Plain CMake (no Conan)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Options: `-DBUILD_TESTS=ON`, `-DBUILD_BENCHMARKS=ON` (both OFF by default). On Windows/MSVC the build is multi-config — always pass `--config Release` to `cmake --build` and `ctest`.

## Test

```bash
cmake -S . -B build -DBUILD_TESTS=ON
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

Tests live in `tests/`. **Project convention (Phase 2):** one `add_executable` per test file — never merge test TUs. Google Test is pulled via `FetchContent`, not Conan. `enable_testing()` must stay in the root `CMakeLists.txt` for MSVC multi-config ctest discovery.

## Layout

- `include/i18ncpp.h` — the **single public header**. Everything users see lives here.
- `src/i18ncpp.cpp` — implementation (parsers, caches, plural rules, interpolation).
- `tests/` — Google Test suites. One target per file.
- `benchmarks/` — micro-benchmarks + `BASELINE.md` (historical captures).
- `examples/` — standalone usage examples.
- `thirdparty/json/` — vendored nlohmann/json submodule.
- `.paul/` — PAUL framework state (PROJECT.md, STATE.md, ROADMAP.md, phases/). **Do not duplicate this content in other docs.**

## Hard constraints

- **C++20 required.** Migrated from C++17 in v0.4 Phase 1. Transparent hashing, `std::span`, `std::format`, and designated-initialized configs all depend on it.
- **Only `nlohmann/json` as an external dependency.** No ICU. No new deps without explicit discussion.
- **No `<regex>`.** Replaced by hand-written manual parsers in v0.3 Phase 6 — the regex dependency is gone and must stay gone (it was a ~14x slowdown on named interpolation).
- **Thread safety: single-writer / single-reader only.** The class is not thread-safe, *not even for `const` methods* — `mutable` caches and scratch buffers are written on every call. The contract is documented at the class boundary in `include/i18ncpp.h` (`\warning` docblock). Sharing an instance across threads requires external synchronization.

## Conventions

- **Transparent hashing** on every `unordered_map` key'd by string — `StringHash` / `StringEqual` with `is_transparent = void`, so `find()` accepts `string_view` with zero allocation.
- **Cache-clear invariant.** Every public mutator must clear both `formatCache_` and `translationCache_`. This is enforced by a parametrized test (`test_cache_invariant.cpp`) that enumerates all 8 public mutators — if you add a mutator, add it to that list.
- **`configure()` uses copy-and-swap.** It stages into a local `FormatConfig tmp`, catches `json::exception`, then commits via move + cache-clear as the nothrow window. Do not regress this — it is AEGIS M1's resolution.
- **JSON boundary errors** are caught as `json::exception` (not just `parse_error`) in `loadLocale` / `mergeLocale` / `load(json)`. Only `I18NError` crosses the public API.
- **Flat storage model.** Runtime holds `unordered_map<locale, unordered_map<flattened_key, string>>`. Nested JSON is flattened at load time — there is no JSON in the hot path.
- **Plural rule lookup** is a constexpr sorted array, not a hash map. Do not replace it with a runtime container.

## Public API entrypoint

Everything is in `i18n::I18N` (see `include/i18ncpp.h`). Key surface:

- `tr(key)` / `tr(key, span)` / `tr(key, initializer_list)` / `trv(key, args...)`
- `trPlural(key, count)` and the parallel span / initializer_list / `trPluralv` variants
- `loadLocaleFromFile` / `loadLocale` / `mergeLocaleFromFile` / `mergeLocale` / `load(json)`
- `setLocale` / `setFallbackLocale` / `getLocale` / `getLocales` / `getFallbackLocale`
- `configure(json)` / `formatNumber` / `formatPrice` / `formatDate`
- `reset()`, `formatCacheSize()`, `translationCacheSize()`

The individual-param `tr` overloads (`tr(key, p1)`, `tr(key, p1, p2)`, …) were **removed in v0.4** in favor of `span` / `initializer_list` / `trv`. Do not reintroduce them.

## Where state lives

- `.paul/PROJECT.md` — project identity, constraints, stakeholders.
- `.paul/STATE.md` — current loop position, recent decisions, deferred issues. **Read this first** when resuming work.
- `.paul/ROADMAP.md` — milestone plan and phase status.
- `.paul/phases/**/` — per-phase PLAN / SUMMARY / APPLY logs.

Read these at session start. Do not restate them here.
