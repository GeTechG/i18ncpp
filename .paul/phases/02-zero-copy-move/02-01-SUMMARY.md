---
phase: 02-zero-copy-move
plan: 01
subsystem: core
tags: [move-semantics, string-view, zero-copy, flattenJson, unordered_set]

requires:
  - phase: 01-benchmarks
    provides: baseline performance measurements identifying hot paths
provides:
  - Move-based JSON string extraction in flattenJson
  - Reusable composite key buffer in tr/trPlural/translate
  - Mutable flattenJson overload for owned JSON data
affects: [03-allocation-optimization, 05-profiling-validation]

tech-stack:
  added: []
  patterns: [composite-key-buffer, const-vs-mutable-overload]

key-files:
  created: []
  modified: [include/i18ncpp.h, src/i18ncpp.cpp]

key-decisions:
  - "Transparent hash deferred: C++17 unordered_map lacks heterogeneous find() — requires C++20"
  - "Dual flattenJson overloads: const for externally-owned JSON (load), mutable for locally-parsed JSON (loadLocale/mergeLocale)"

patterns-established:
  - "Composite key buffer: declare once, assign+append per lookup instead of operator+ concatenation"
  - "Mutable vs const overload: move from owned data, copy from borrowed data"

duration: 15min
started: 2026-04-09T00:00:00Z
completed: 2026-04-09T00:15:00Z
---

# Phase 2 Plan 01: Zero-copy & Move Semantics Summary

**Move semantics for JSON string extraction and reusable composite key buffers to reduce allocations in data loading and lookup hot paths**

## Performance

| Metric | Value |
|--------|-------|
| Duration | ~15min |
| Tasks | 3 completed |
| Files modified | 2 |
| Tests | 82/82 passing |

## Acceptance Criteria Results

| Criterion | Status | Notes |
|-----------|--------|-------|
| AC-1: Heterogeneous map lookup | Fail (deferred) | C++17 `unordered_map::find()` does not support transparent/heterogeneous lookup — requires C++20. Deferred. |
| AC-2: Move semantics for JSON extraction | Pass | Mutable `flattenJson(json&&)` overload moves strings via `get_ref<string_t&>()` + `std::move()` |
| AC-3: All existing tests pass | Pass | 82/82 tests pass, zero regressions |

## Accomplishments

- Added mutable `flattenJson` overload that moves strings from parsed JSON instead of copying — eliminates one string allocation per translation entry during `loadLocale`/`mergeLocale`
- Replaced per-lookup `operator+` string concatenation with reusable `compositeKey` buffer in `tr`, `trPlural`, and `translate` — reduces heap allocations in the lookup hot path
- Replaced `unordered_map<string, bool>` with `unordered_set<string>` in `getFallbacks` and moved ancestry strings instead of copying
- Minor `setLocale` improvement: move string into vector instead of copy

## Files Created/Modified

| File | Change | Purpose |
|------|--------|---------|
| `include/i18ncpp.h` | Modified | Added mutable `flattenJson(json&&)` overload declaration |
| `src/i18ncpp.cpp` | Modified | Mutable flattenJson impl, composite key buffers in tr/trPlural/translate, unordered_set in getFallbacks, move in setLocale |

## Decisions Made

| Decision | Rationale | Impact |
|----------|-----------|--------|
| Defer transparent hash/heterogeneous lookup | C++17 `unordered_map::find()` only accepts `const Key&` — transparent `find<K>()` is C++20 | AC-1 not met; recorded as deferred issue for future C++20 migration |
| Dual flattenJson overloads (const + mutable) | `load(const json&)` receives externally-owned data that must not be moved from; `loadLocale`/`mergeLocale` parse locally and can move | Clean separation of borrowed vs owned data |

## Deviations from Plan

### Summary

| Type | Count | Impact |
|------|-------|--------|
| Scope reduction | 1 | AC-1 deferred — C++17 limitation discovered during execution |
| Auto-fixed | 0 | — |
| Deferred | 1 | Transparent hash for C++20 |

**Total impact:** Primary optimization (transparent hash) blocked by language standard constraint. Move semantics and buffer reuse delivered as planned.

### Deferred Items

- Transparent hash heterogeneous lookup: requires C++20 `template<class K> iterator find(const K&)` on `std::unordered_map`. Deferred until C++20 migration decision.

## Next Phase Readiness

**Ready:**
- Move semantics pattern established for data loading paths
- Composite key buffer pattern available for other hot paths
- All tests passing, no regressions

**Concerns:**
- AC-1 (biggest potential win for lookup) is blocked by C++17 constraint
- Interpolation (regex-based) remains the #1 bottleneck — untouched per scope limits

**Blockers:**
- None

---
*Phase: 02-zero-copy-move, Plan: 01*
*Completed: 2026-04-09*
