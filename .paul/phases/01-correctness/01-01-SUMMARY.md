---
phase: 01-correctness
plan: 01
subsystem: core
tags: [i18n, bugfix, load, format-config, bool, keyExists]

requires:
  - phase: none
    provides: n/a
provides:
  - F1–F4 correctness fixes from REVIEW-2026-04-11
  - G1–G4 regression tests (11 new test cases)
affects: []

tech-stack:
  added: []
  patterns:
    - baselineConfig_ pattern for format config isolation on locale switch

key-files:
  created:
    - tests/test_load_replace.cpp
    - tests/test_format_isolation.cpp
    - tests/test_bool_conversion.cpp
    - tests/test_key_exists.cpp
  modified:
    - src/i18ncpp.cpp
    - include/i18ncpp.h
    - tests/CMakeLists.txt

key-decisions:
  - "F2: baselineConfig_ member preserves pristine defaults; configure() does not touch it"
  - "F4: keyExists checks .other suffix only (matches tr fallback), not full trPlural resolution"

patterns-established:
  - "baselineConfig_ for format isolation: setLocale resets to baseline when no per-locale config exists"

duration: ~15min
completed: 2026-04-12
---

# Phase 1 Plan 01: Correctness Fixes Summary

**Fixed 4 REVIEW-2026-04-11 correctness bugs (F1–F4) with 11 regression tests across 4 new test files.**

## Acceptance Criteria Results

| Criterion | Status | Notes |
|-----------|--------|-------|
| AC-1: load(json) replaces locale data | Pass | localesData[locale].clear() before flatten |
| AC-2: Format config resets on locale switch | Pass | baselineConfig_ restores defaults when no per-locale config |
| AC-3: trv/trPluralv serialize bool as "true"/"false" | Pass | bool branch moved above arithmetic in toString |
| AC-4: keyExists resolves plural/variant suffixes | Pass | .other suffix check added to fallback loop |

## Files Created/Modified

| File | Change | Purpose |
|------|--------|---------|
| src/i18ncpp.cpp | Modified | F1: .clear() in load(); F2: baselineConfig_ in setLocale/reset; F4: .other in keyExists |
| include/i18ncpp.h | Modified | F2: baselineConfig_ member; F3: bool branch reorder in toString |
| tests/test_load_replace.cpp | Created | G1: 2 tests — stale key removal + other locales unaffected |
| tests/test_format_isolation.cpp | Created | G2: 3 tests — reset to baseline, restore on switch-back, vector overload |
| tests/test_bool_conversion.cpp | Created | G3: 3 tests — trv bool, trPluralv bool, integers still work |
| tests/test_key_exists.cpp | Created | G4: 3 tests — exact key, plural base via .other, fallback locale |
| tests/CMakeLists.txt | Modified | 4 new add_executable targets + gtest_discover_tests |

## Deviations from Plan

### Auto-fixed Issues

**1. Test placeholder indexing (G3)**
- **Found during:** Task 3 qualify
- **Issue:** Tests used 1-based placeholders ({1}) but interpolateArray uses 0-based ({0})
- **Fix:** Changed test fixtures to {0}-based indexing
- **Verification:** All 3 BoolConversion tests pass after fix

### Deferred Items
None new. Existing deferred items unchanged.

## Verification Results

```
109/109 tests passed, 0 failures
Build: cmake --build build --config Release — clean
```

## Next Phase Readiness

**Ready:**
- All F1–F4 correctness findings closed
- G1–G4 regression coverage in place
- 109 tests passing, no regressions

**Concerns:**
- None

**Blockers:**
- None

---
*Phase: 01-correctness, Plan: 01*
*Completed: 2026-04-12*
