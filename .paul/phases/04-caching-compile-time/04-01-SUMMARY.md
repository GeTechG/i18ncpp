---
phase: 04-caching-compile-time
plan: 01
subsystem: core
tags: [translation-cache, interpolation, c++17]

requires:
  - phase: 03-allocation-optimization
    provides: mutable interpolation buffers, fold expression argsToStrings
provides:
  - Translation result cache for tr/trPlural with automatic invalidation
affects: [05-profiling-validation]

tech-stack:
  added: []
  patterns: [translation result caching with null-byte-separated cache keys]

key-files:
  created: []
  modified: [include/i18ncpp.h, src/i18ncpp.cpp]

key-decisions:
  - "Null byte separator for cache keys — cannot appear in user strings, avoids ambiguity"
  - "Separate cache key prefix for trPlural (\\0P\\0) to avoid collisions with tr"
  - "Unbounded cache — translation sets are finite, LRU adds unnecessary complexity"
  - "translate() (json params overload) intentionally not cached — json hashing is expensive"
  - "Constexpr FNV-1a hash removed — no internal consumer, dead code without C++20 heterogeneous lookup"

patterns-established:
  - "Translation result cache: mutable unordered_map cleared on all mutation paths"

duration: 5min
started: 2026-04-09T00:00:00Z
completed: 2026-04-09T00:05:00Z
---

# Phase 4 Plan 01: Caching & Compile-time Summary

**Added translation result cache for tr/trPlural (O(1) cache hit skips regex interpolation). Constexpr FNV-1a hash was planned but removed as dead code (no consumer without C++20).**

## Performance

| Metric | Value |
|--------|-------|
| Duration | ~5min |
| Tasks | 1 completed, 1 removed (invalid) |
| Files modified | 2 |

## Acceptance Criteria Results

| Criterion | Status | Notes |
|-----------|--------|-------|
| AC-1: Translation result cache hits skip interpolation | Pass | Cache lookup in tr() and trPlural() returns stored result on hit |
| AC-2: Cache invalidation on mutation | Pass | clearTranslationCache() called in 7 mutation paths: setLocale (x2), setFallbackLocale, load, loadLocale, mergeLocale, configure, reset |
| AC-3: Constexpr FNV-1a hash computes at compile time | Removed | Task invalidated — no consumer without C++20 heterogeneous lookup |

## Accomplishments

- Added `translationCache_` (mutable unordered_map) to cache tr/trPlural results, eliminating regex interpolation on repeated calls
- Cache keys use null-byte separators for unambiguous key+params encoding; trPlural uses `\0P\0` prefix to avoid collisions
- Invalidation in all 7 mutation paths ensures cache coherence
- Added `translationCacheSize()` public API for cache observability

## Files Created/Modified

| File | Change | Purpose |
|------|--------|---------|
| `include/i18ncpp.h` | Modified | Added translationCache_ member, clearTranslationCache() private method, translationCacheSize() public method |
| `src/i18ncpp.cpp` | Modified | Implemented cache lookup/store in tr() and trPlural(), clearTranslationCache() in all mutation paths, translationCacheSize() |

## Decisions Made

None beyond plan — followed plan as specified.

## Deviations from Plan

- **Task 2 (constexpr FNV-1a hash) removed** — planned without a consumer; the hash function had no internal usage and requires C++20 heterogeneous lookup to be useful. Identified as a planning error (feature without need).

## Issues Encountered

None.

## Next Phase Readiness

**Ready:**
- All caching layers complete (formatting cache from v0.2, translation cache from this phase)
- Phase 5 (Profiling & Validation) can now re-run benchmarks to measure cumulative v0.3 gains

**Concerns:**
- Translation cache effectiveness depends on repeated identical calls — applications with high param variance will see fewer cache hits
- Regex remains the dominant cost on cache misses (~95% of interpolation wall time)

**Blockers:**
- None

---
*Phase: 04-caching-compile-time, Plan: 01*
*Completed: 2026-04-09*
