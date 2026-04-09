# i18ncpp

## What This Is

A lightweight C++ internationalization (i18n) library — a port of smiti18n (Lua) to C++. Provides translation management from JSON files, variable interpolation, pluralization with rules for many languages, locale fallbacks, nested keys, contextual variants, and locale-aware formatting for numbers, currencies, and dates/times.

## Core Value

C++ developers can localize their applications with translation, pluralization, and locale-aware formatting — without heavy dependencies like ICU.

## Current State

| Attribute | Value |
|-----------|-------|
| Type | Application (C++ library) |
| Version | 0.3.0-dev |
| Status | In Progress |
| Last Updated | 2026-04-09 |

## Requirements

### Core Features

- Load and manage translations from JSON files with multiple locales, fallback locale, and merge support
- Variable interpolation (positional, named, formatted) and pluralization with language-specific rules
- Locale-aware formatting for numbers, currencies, and date/time
- Nested translation keys (dot notation) and contextual variants

### Validated (Shipped)
- Test coverage for translation basics (tr, nested keys, fallback, locale switching) — Phase 1
- Test coverage for pluralization (en, ru, fr, pl, cs, ar rules) — Phase 1
- Test coverage for interpolation (positional, unnamed, named, formatted) — Phase 1
- Test coverage for formatting (number, currency, date/time with named and custom patterns) — Phase 1
- Test coverage for locale management (merge, load-from-file, reset) — Phase 1
- Flat storage with O(1) key lookup (replaced nested JSON traversal) — Phase 2
- Formatting result cache with automatic invalidation on locale/config change — Phase 3
- Benchmark suite with 18 micro-benchmarks for baseline performance measurement — v0.3 Phase 1
- Move semantics for JSON string extraction in data loading (flattenJson mutable overload) — v0.3 Phase 2
- Composite key buffer reuse in lookup hot paths (tr, trPlural, translate) — v0.3 Phase 2
- Mutable member buffer pooling for interpolation (interpolateBuf_, interpolateBuf2_, extendedParamsBuf_) — v0.3 Phase 3
- O(n) argsToStrings via C++17 fold expression (replaced O(n^2) recursive insert-at-begin) — v0.3 Phase 3
- Translation result cache for tr/trPlural with automatic invalidation on mutation — v0.3 Phase 4
- Constexpr FNV-1a hash utility for compile-time key hashing — v0.3 Phase 4

### Active (In Progress)
- String view / zero-copy for key lookup (deferred: requires C++20 heterogeneous find)

### Planned (Next)
- Profiling & validation (re-run benchmarks, compare with baseline)

### Out of Scope
- ICU dependency
- Contextual variant testing (lower priority, deferred)

## Constraints

### Technical Constraints
- Only nlohmann/json as external dependency

### Business Constraints
- None identified

## Key Decisions

| Decision | Rationale | Date | Status |
|----------|-----------|------|--------|
| C++17 standard | Modern features without cutting-edge compiler requirements | 2026-04-09 | Active |
| nlohmann/json for JSON | Widely used, header-only, well-tested | 2026-04-09 | Active |
| CMake + Conan | Standard C++ build and package management | 2026-04-09 | Active |
| Flat unordered_map storage | O(1) lookup vs O(depth) JSON traversal; plural/variant forms as composite keys | 2026-04-09 | Active |
| Leaf object heuristic | Objects with all-string values are plural/variant forms, not namespace containers | 2026-04-09 | Active |
| Formatting cache (mutable) | Cache formatNumber/Price/Date results; invalidate on locale/config change | 2026-04-09 | Active |
| Transparent hash deferred | C++17 unordered_map lacks heterogeneous find(); deferred to C++20 migration | 2026-04-09 | Deferred |
| Dual flattenJson overloads | const for borrowed JSON (load), mutable for owned JSON (loadLocale/mergeLocale) | 2026-04-09 | Active |
| Mutable buffer pooling | Reuse interpolation buffers as mutable members; clear+reserve instead of local construction | 2026-04-09 | Active |
| Fold expression for variadics | Single-pass O(n) parameter pack expansion replaces recursive O(n^2) insert-at-begin | 2026-04-09 | Active |
| Translation result cache | Cache tr/trPlural results keyed by null-byte-separated key+params; invalidate on all mutation paths | 2026-04-09 | Active |
| Constexpr FNV-1a hash | Header-only compile-time hash for keys; deferred internal map usage to C++20 | 2026-04-09 | Active |

## Success Metrics

| Metric | Target | Current | Status |
|--------|--------|---------|--------|
| Correctness | All translations resolve correctly | 82 tests passing | Complete |
| Performance | Fast locale resolution and formatting | O(1) hash lookup + formatting cache + translation result cache | In Progress |
| Benchmark baseline | Measure all hot paths before optimization | 18 benchmarks captured | Complete |

## Tech Stack / Tools

| Layer | Technology | Notes |
|-------|------------|-------|
| Language | C++17 | Modern standard |
| Build | CMake | Cross-platform build |
| Package Manager | Conan | Dependency management |
| JSON | nlohmann/json | Only external dependency |
| Testing | Google Test v1.14.0 | Via FetchContent |

---
*Last updated: 2026-04-09 after v0.3 Phase 4 (Caching & Compile-time complete)*
