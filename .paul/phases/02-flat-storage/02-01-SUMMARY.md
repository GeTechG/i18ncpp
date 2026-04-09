---
phase: 02-flat-storage
plan: 01
subsystem: core
tags: [unordered_map, flat-storage, performance, refactoring]

requires:
  - phase: 01-tests
    provides: 75 tests covering all public API surfaces for behavioral equivalence validation
provides:
  - Flat string storage replacing nested JSON traversal
  - O(1) key lookup via unordered_map
  - flattenJson helper for load-time JSON flattening
affects: []

tech-stack:
  added: []
  patterns: [flat-key storage with dot-separated composite keys for plurals/variants]

key-files:
  created: []
  modified: [include/i18ncpp.h, src/i18ncpp.cpp]

key-decisions:
  - "Leaf object detection: objects where all values are strings are flattened as key.subkey entries (plural/variant forms)"
  - "Array flattening: array elements stored as key.0, key.1, etc."
  - "Plural/variant resolution moved inline into tr/trPlural/translate — no separate handler methods"

patterns-established:
  - "Composite key pattern: plural forms accessed as key.one, key.other; arrays as key.0, key.1"
  - "Single hash lookup for all translation access — no tree traversal"

duration: ~25min
started: 2026-04-09T00:00:00Z
completed: 2026-04-09T00:30:00Z
---

# Phase 2 Plan 01: Flat Storage Refactoring Summary

**Replaced nested JSON tree traversal with flat `unordered_map<string, string>` per locale, achieving O(1) key lookup with pure string values — no JSON in runtime storage.**

## Performance

| Metric | Value |
|--------|-------|
| Duration | ~25min |
| Tasks | 3 completed |
| Files modified | 2 |
| Net lines | -119 (160 added, 279 removed) |

## Acceptance Criteria Results

| Criterion | Status | Notes |
|-----------|--------|-------|
| AC-1: Flat string storage | Pass | `localesData` is `unordered_map<string, unordered_map<string, string>>`. Plural forms flattened as `key.one`/`key.other`, arrays as `key.0`/`key.1`. No JSON objects in storage. |
| AC-2: O(1) key lookup | Pass | `getTranslationData` performs two hash lookups (locale + key) with no dot-splitting or tree traversal. |
| AC-3: All existing tests pass | Pass | All 75 tests pass (0.41s total). |

## Accomplishments

- Storage type changed from `unordered_map<string, json>` to `unordered_map<string, unordered_map<string, string>>` — pure strings only
- `flattenJson` helper flattens nested JSON at load time, handling objects, leaf objects (plural/variant), and arrays
- `getTranslationData` reduced from ~70-line tree traversal to ~15-line double hash lookup returning `const string*`
- Removed `dotSplit`, `handlePlural`, `handlePluralWithArray`, `handleVariant` — plural/variant logic inlined into `trPlural`/`translate`
- Net reduction of 119 lines while maintaining full behavioral equivalence

## Files Created/Modified

| File | Change | Purpose |
|------|--------|---------|
| `include/i18ncpp.h` | Modified | Changed `localesData` type, added `flattenJson` declaration, changed `getTranslationData` return type to `const string*`, removed dead method declarations |
| `src/i18ncpp.cpp` | Modified | Implemented `flattenJson`, rewrote `loadLocale`/`mergeLocale`/`load`/`getTranslationData`/`tr`/`trPlural`/`translate`/`keyExists`, removed `dotSplit`/`handlePlural`/`handlePluralWithArray`/`handleVariant` |

## Decisions Made

| Decision | Rationale | Impact |
|----------|-----------|--------|
| Inline plural/variant logic | With flat storage, plural resolution is just building a composite key — separate handler methods add indirection without value | Simpler code, fewer methods, same behavior |
| Return `const string*` from `getTranslationData` | No mutation needed on stored translations; pointer allows null check for missing keys | Cleaner API, const-correct |
| Leaf object heuristic (all-string-values) | Distinguishes plural/variant objects from namespace containers without requiring schema | Correct flattening of mixed-depth JSON |

## Deviations from Plan

None — plan executed exactly as written.

## Issues Encountered

None.

## Next Phase Readiness

**Ready:**
- Flat storage foundation in place for any future caching layer (Phase 3)
- All 75 tests provide regression safety net

**Concerns:**
- None

**Blockers:**
- None

---
*Phase: 02-flat-storage, Plan: 01*
*Completed: 2026-04-09*
