---
phase: 02-transparent-hash
plan: 01
subsystem: core
tags: [c++20, transparent-hash, heterogeneous-lookup, string_view, zero-allocation]

requires:
  - phase: 01-cpp20-migration
    provides: C++20 compilation support (heterogeneous find() available)
provides:
  - Transparent hash/equal types (StringHash, StringEqual) for zero-allocation map lookups
  - All unordered_maps use heterogeneous lookup
  - getTranslationData performs zero-allocation lookups via string_view
affects: [03-format-constexpr, 04-api-modernization]

tech-stack:
  added: []
  patterns: [transparent hashing with is_transparent tag, heterogeneous find()]

key-files:
  modified: [include/i18ncpp.h, src/i18ncpp.cpp]

key-decisions:
  - "Single StringHash using std::hash<string_view> for both string and string_view inputs"
  - "StringEqual accepts string_view pairs — implicit conversion handles mixed types"

patterns-established:
  - "All new unordered_maps must use StringHash, StringEqual template params"
  - "Map lookups use string_view directly — no temporary std::string for find()"

duration: ~5min
completed: 2026-04-09T14:00:00Z
---

# Phase 2 Plan 01: Transparent Hash & Zero-Copy Summary

**StringHash/StringEqual types enable zero-allocation heterogeneous lookup on all 5 unordered_maps**

## Performance

| Metric | Value |
|--------|-------|
| Duration | ~5min |
| Completed | 2026-04-09 |
| Tasks | 2 completed |
| Files modified | 2 |

## Acceptance Criteria Results

| Criterion | Status | Notes |
|-----------|--------|-------|
| AC-1: Transparent Hash Type Defined | Pass | StringHash + StringEqual with is_transparent tag |
| AC-2: All Maps Use Transparent Hash | Pass | localesData (both levels), formatConfigs, formatCache_, translationCache_ |
| AC-3: Zero-Allocation Lookups | Pass | getTranslationData uses string_view directly — no std::string temporaries |
| AC-4: All Tests Pass | Pass | 82/82 tests pass |

## Accomplishments

- Defined StringHash and StringEqual structs with `is_transparent` tag for C++20 heterogeneous lookup
- Updated all 5 unordered_map declarations (localesData outer+inner, formatConfigs, formatCache_, translationCache_) to use transparent hash
- Eliminated 2 temporary std::string allocations per getTranslationData call (the hot path)
- Updated flattenJson signatures and temp map to match new inner map type

## Files Created/Modified

| File | Change | Purpose |
|------|--------|---------|
| `include/i18ncpp.h` | Modified | Added StringHash/StringEqual structs, updated 5 map declarations + 2 flattenJson signatures |
| `src/i18ncpp.cpp` | Modified | Updated flattenJson definitions, tempFlat type, removed string temporaries in getTranslationData |

## Deviations from Plan

None - plan executed exactly as written.

## Issues Encountered

None.

## Next Phase Readiness

**Ready:**
- All maps now support heterogeneous lookup — foundation for further string_view optimizations
- Zero test regressions confirm backward compatibility

**Concerns:**
- None

**Blockers:**
- None

---
*Phase: 02-transparent-hash, Plan: 01*
*Completed: 2026-04-09*
