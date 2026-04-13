# Milestones

Completed milestone log for this project.

| Milestone | Completed | Duration | Stats |
|-----------|-----------|----------|-------|
| v0.4.2 Correctness & Portability | 2026-04-13 | 2 days | 2 phases, 2 plans |
| v0.4.1 Hardening & Infrastructure | 2026-04-11 | 1 day | 4 phases, 5 plans |
| v0.4 C++20 Modernization | 2026-04-09 | 1 day | 4 phases, 4 plans |
| v0.3 Deep Performance | 2026-04-09 | 1 day | 6 phases, 6 plans |
| v0.2 Performance & Reliability | Pre-tracking | — | 3 phases, 5 plans |
| v0.1 Initial Release | Pre-tracking | — | — |

---

## v0.4.2 Correctness & Portability

**Completed:** 2026-04-13
**Duration:** 2 days (2 phases)

### Stats

| Metric | Value |
|--------|-------|
| Phases | 2 |
| Plans | 2 |
| Files changed | ~10 |

### Key Accomplishments

- F1: `load(json)` replace semantics — locale map cleared before flatten; no stale keys survive reload
- F2: Format config isolation via `baselineConfig_` — `setLocale` resets to pristine defaults when a locale has no `_formats`
- F3: Bool serialization fix in `trv` / `trPluralv` — bool branch precedes `is_arithmetic_v` check in `toString`
- F4: `keyExists` parity with `tr` — `.other` suffix probed during fallback resolution
- G1–G4: 11 new regression tests pinning the correctness fixes
- Portable localtime shim (`localtime_s` on Win32, `localtime_r` on POSIX)
- `std::format` / `{fmt}` fallback via `i18n_fmt` namespace alias + CMake `FetchContent` probe
- PROJECT.md dependency constraint amended to allow `{fmt}` as conditional fallback

### Key Decisions

| Decision | Rationale |
|----------|-----------|
| `baselineConfig_` member | Preserves pristine format defaults; `configure()` does not touch it |
| `keyExists` checks only `.other` suffix | Matches `tr()` fallback; does NOT replicate full `trPlural` plural-form resolution |
| `std::format` fallback over hard floor | Supports older toolchains without forcing a C++20 compiler bump |
| `{fmt}` via CMake FetchContent, not Conan | Build-system-only concern; Conan users unaffected |
| `i18n_fmt` namespace alias | Type-safe abstraction; no preprocessor hazards at call sites |

---

## v0.4 C++20 Modernization

**Completed:** 2026-04-09
**Duration:** 1 day (all 4 phases)

### Stats

| Metric | Value |
|--------|-------|
| Phases | 4 |
| Plans | 4 |
| Files changed | 6 |

### Key Accomplishments

- C++20 build migration with aggregate initialization fix for NumberConfig
- StringHash/StringEqual transparent hash types enabling zero-allocation heterogeneous lookup on all 5 unordered_maps
- std::format for cache key construction, constexpr sorted array for plural rule lookup (eliminates heap allocation)
- %B date format specifier support fixing broken default long_date pattern
- std::span replaces std::vector in tr/trPlural public API — zero-copy parameter passing
- API surface simplified from 10 to 6 overloads; initializer_list convenience retained

### Key Decisions

| Decision | Rationale |
|----------|-----------|
| Explicit NumberConfig constructor | C++20 aggregate init rules changed — structs with user-declared special members need explicit constructors |
| Single StringHash via string_view | std::hash<string_view> handles both string and string_view inputs uniformly |
| Linear scan for plural rules | 32 entries — simpler than binary search, cache-friendly, zero overhead |
| %B as %F alias | Standard strftime compatibility for full month name |
| Keep initializer_list overloads | std::span can't bind brace-init directly in C++20 |

---

## v0.3 Deep Performance

**Completed:** 2026-04-09
**Duration:** 1 day (all 6 phases)

### Stats

| Metric | Value |
|--------|-------|
| Phases | 6 |
| Plans | 6 |
| Files changed | 9 |

### Key Accomplishments

- Custom benchmark harness with 18 micro-benchmarks establishing baseline performance data
- Move semantics for JSON string extraction and reusable composite key buffers in lookup hot paths
- Mutable member buffer pooling for interpolation; O(n) fold expression replacing O(n^2) recursive argsToStrings
- Translation result cache (tr/trPlural) with automatic invalidation on all 7 mutation paths
- Manual parsers replacing all 4 std::regex patterns — `<regex>` dependency fully removed
- InterpolateNamed: 6740 -> 489 ns/op (93% faster)
- Lookup: 87-96% faster across all benchmarks (TrSimpleKey 205 -> 22 ns/op)
- Positional interpolation: 94-96% faster (1545 -> 65 ns/op)

### Key Decisions

| Decision | Rationale |
|----------|-----------|
| Custom chrono harness over Google Benchmark | Zero new dependencies |
| Transparent hash deferred to C++20 | C++17 unordered_map lacks heterogeneous find() |
| Dual flattenJson overloads (const + mutable) | Borrowed vs owned data separation |
| Null-byte cache key separators | Unambiguous encoding, cannot appear in user strings |
| Manual parsers over std::regex | 93% improvement; removes entire `<regex>` dependency |

---

## v0.2 Performance & Reliability

**Completed:** Pre-tracking
**Phases:** Tests, Flat Storage, Formatting Cache

---

## v0.1 Initial Release

**Completed:** Pre-tracking
**Status:** Shipped

---
*Created: 2026-04-09*
