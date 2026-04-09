---
phase: 04-api-modernization
plan: 01
subsystem: api
tags: [std::span, C++20, overload-simplification]

requires:
  - phase: 03-format-constexpr
    provides: C++20 build configuration, std::format usage
provides:
  - std::span-based tr/trPlural public API
  - Simplified overload set (10 → 6)
  - initializer_list convenience overloads
affects: []

tech-stack:
  added: [std::span]
  patterns: [span-based parameter passing for contiguous ranges]

key-files:
  created: []
  modified: [include/i18ncpp.h, src/i18ncpp.cpp, tests/test_interpolation.cpp, tests/test_plural.cpp, benchmarks/bench_interpolation.cpp]

key-decisions:
  - "Keep initializer_list overloads for brace-init syntax convenience"
  - "Prefer initializer_list syntax at call sites over trv for literal strings"

patterns-established:
  - "std::span<const std::string> as canonical parameter type for string arrays"

duration: 5min
started: 2026-04-09T00:00:00Z
completed: 2026-04-09T00:00:00Z
---

# Phase 4 Plan 01: API Modernization Summary

**std::span replaces std::vector in tr/trPlural public API; 4 individual-param overloads removed, reducing API surface from 10 to 6 overloads**

## Performance

| Metric | Value |
|--------|-------|
| Duration | ~5min |
| Tasks | 2 completed |
| Files modified | 5 |

## Acceptance Criteria Results

| Criterion | Status | Notes |
|-----------|--------|-------|
| AC-1: std::span replaces vector in public API | Pass | Core tr/trPlural overloads accept std::span<const std::string> |
| AC-2: Individual parameter overloads removed | Pass | Removed tr(key,p1), tr(key,p1,p2), tr(key,p1,p2,p3), trPlural(key,n,p1), trPlural(key,n,p1,p2) |
| AC-3: All tests pass with updated API | Pass | 82/82 tests pass, benchmarks compile and run |

## Accomplishments

- Replaced `const std::vector<std::string>&` with `std::span<const std::string>` in tr, trPlural, and interpolateArray
- Removed 4 individual-param overloads (tr: 3 removed, trPlural: 2 removed) — API surface 10 → 6
- Added `std::initializer_list<std::string>` overloads for ergonomic brace-init syntax `tr("key", {"a", "b"})`
- Updated 7 call sites across tests (3) and benchmarks (3) to use initializer_list syntax

## Files Created/Modified

| File | Change | Purpose |
|------|--------|---------|
| `include/i18ncpp.h` | Modified | Added `#include <span>`, replaced vector params with span, removed individual-param overloads, updated interpolateArray signature |
| `src/i18ncpp.cpp` | Modified | Updated tr/trPlural/interpolateArray implementations to use span, removed deleted overload implementations, added initializer_list→span delegation |
| `tests/test_interpolation.cpp` | Modified | Updated 3 call sites from individual-param to initializer_list syntax |
| `tests/test_plural.cpp` | Modified | Updated 2 call sites from individual-param to initializer_list syntax |
| `benchmarks/bench_interpolation.cpp` | Modified | Updated 3 call sites from individual-param to initializer_list syntax |

## Decisions Made

| Decision | Rationale | Impact |
|----------|-----------|--------|
| Keep initializer_list overloads | std::span cannot bind to `{"a", "b"}` brace-init directly in C++20 | Provides ergonomic API for literal string args |
| Prefer initializer_list at call sites | More readable than trv for literal string arguments | Test/benchmark code uses `{"Alice", "Wonderland"}` style |

## Deviations from Plan

None — plan executed exactly as written.

## Issues Encountered

None.

## Next Phase Readiness

**Ready:**
- v0.4 C++20 Modernization milestone complete — all 4 phases done
- Full test suite (82 tests) passing
- All C++20 features adopted: modules-compatible build, transparent hash, std::format, constexpr, std::span

**Concerns:**
- None

**Blockers:**
- None

---
*Phase: 04-api-modernization, Plan: 01*
*Completed: 2026-04-09*
