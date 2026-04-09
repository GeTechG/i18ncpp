# i18ncpp

## What This Is

A lightweight C++ internationalization (i18n) library — a port of smiti18n (Lua) to C++. Provides translation management from JSON files, variable interpolation, pluralization with rules for many languages, locale fallbacks, nested keys, contextual variants, and locale-aware formatting for numbers, currencies, and dates/times.

## Core Value

C++ developers can localize their applications with translation, pluralization, and locale-aware formatting — without heavy dependencies like ICU.

## Current State

| Attribute | Value |
|-----------|-------|
| Type | Application (C++ library) |
| Version | 0.3.0 |
| Status | Stable (v0.4 shipped) |
| Last Updated | 2026-04-09 |

## Requirements

### Core Features

- Load and manage translations from JSON files with multiple locales, fallback locale, and merge support
- Variable interpolation (positional, named, formatted) and pluralization with language-specific rules
- Locale-aware formatting for numbers, currencies, and date/time
- Nested translation keys (dot notation) and contextual variants

### Validated (Shipped)
- C++20 build configuration with full backward compatibility — v0.4 Phase 1
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
- Profiling & validation: all optimizations measured, 87-96% lookup/interpolation improvement confirmed — v0.3 Phase 5
- Manual parsers replace all std::regex patterns; `<regex>` dependency removed; InterpolateNamed 6740→489 ns/op (93% faster) — v0.3 Phase 6
- Transparent hash (StringHash/StringEqual) with zero-allocation string_view lookups on all unordered_maps — v0.4 Phase 2
- std::format for cache key construction, constexpr plural rule lookup, %B date specifier fix — v0.4 Phase 3
- std::span parameters for tr/trPlural, simplified overload set (10→6), initializer_list convenience — v0.4 Phase 4

### Active (In Progress)
- None

### Planned (Next)
- None

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
| C++20 standard | Unlocks heterogeneous lookup, std::format, std::span, constexpr improvements | 2026-04-09 | Active |
| nlohmann/json for JSON | Widely used, header-only, well-tested | 2026-04-09 | Active |
| CMake + Conan | Standard C++ build and package management | 2026-04-09 | Active |
| Flat unordered_map storage | O(1) lookup vs O(depth) JSON traversal; plural/variant forms as composite keys | 2026-04-09 | Active |
| Leaf object heuristic | Objects with all-string values are plural/variant forms, not namespace containers | 2026-04-09 | Active |
| Formatting cache (mutable) | Cache formatNumber/Price/Date results; invalidate on locale/config change | 2026-04-09 | Active |
| Transparent hash implemented | StringHash/StringEqual with is_transparent enable zero-allocation find() on all maps | 2026-04-09 | Active |
| NumberConfig explicit constructor | C++20 aggregate init rules require explicit parameterized constructor for structs with user-declared special members | 2026-04-09 | Active |
| Dual flattenJson overloads | const for borrowed JSON (load), mutable for owned JSON (loadLocale/mergeLocale) | 2026-04-09 | Active |
| Mutable buffer pooling | Reuse interpolation buffers as mutable members; clear+reserve instead of local construction | 2026-04-09 | Active |
| Fold expression for variadics | Single-pass O(n) parameter pack expansion replaces recursive O(n^2) insert-at-begin | 2026-04-09 | Active |
| Translation result cache | Cache tr/trPlural results keyed by null-byte-separated key+params; invalidate on all mutation paths | 2026-04-09 | Active |
| Manual parsers over regex | Hand-written character scanners replace std::regex for interpolation; removes `<regex>` dependency entirely | 2026-04-09 | Active |
| std::format for cache keys | Cleaner cache key construction; replaces manual string concatenation with std::to_string | 2026-04-09 | Active |
| Constexpr plural rule lookup | static constexpr std::array replaces runtime unordered_map; eliminates heap allocation | 2026-04-09 | Active |
| %B as %F alias | %B maps to full month name (standard strftime); fixes default long_date pattern | 2026-04-09 | Active |
| std::span for tr/trPlural params | Zero-copy parameter passing from any contiguous container; replaces vector-only API | 2026-04-09 | Active |
| Simplified overload set | Removed individual-param overloads (1/2/3 string args); initializer_list + trv cover all cases | 2026-04-09 | Active |

## Success Metrics

| Metric | Target | Current | Status |
|--------|--------|---------|--------|
| Correctness | All translations resolve correctly | 82 tests passing | Complete |
| Performance | Fast locale resolution and formatting | O(1) hash lookup + formatting cache + translation cache + manual parsers | Complete |
| Benchmark baseline | Measure all hot paths before optimization | 18 benchmarks with before/after comparison | Complete |

## Tech Stack / Tools

| Layer | Technology | Notes |
|-------|------------|-------|
| Language | C++20 | Migrated from C++17 in v0.4 Phase 1 |
| Build | CMake | Cross-platform build |
| Package Manager | Conan | Dependency management |
| JSON | nlohmann/json | Only external dependency |
| Testing | Google Test v1.14.0 | Via FetchContent |

---
*Last updated: 2026-04-09 after Phase 4 (API Modernization) — v0.4 milestone complete*
