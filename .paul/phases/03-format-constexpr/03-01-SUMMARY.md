---
phase: 03-format-constexpr
plan: 01
subsystem: core
tags: [std::format, constexpr, date-formatting, c++20]

requires:
  - phase: 02-transparent-hash
    provides: C++20 build configuration, transparent hash infrastructure
provides:
  - std::format cache key construction
  - constexpr plural rule lookup
  - %B date format specifier support
affects: [04-api-modernization]

tech-stack:
  added: [std::format, std::array constexpr]
  patterns: [constexpr lookup tables]

key-files:
  modified: [src/i18ncpp.cpp]

key-decisions:
  - "Linear scan over constexpr sorted array for plural rules (32 entries, simpler than binary search)"
  - "%B maps to same logic as %F (full month name), maintaining strftime compatibility"

patterns-established:
  - "constexpr std::array for small lookup tables instead of runtime unordered_map"

duration: 5min
completed: 2026-04-09T00:00:00Z
---

# Phase 3 Plan 01: Format & Constexpr Summary

**std::format for cache keys, constexpr plural rule lookup, and %B date specifier fix — completing C++20 modernization of core internals.**

## Performance

| Metric | Value |
|--------|-------|
| Duration | ~5min |
| Completed | 2026-04-09 |
| Tasks | 3 completed |
| Files modified | 1 |

## Acceptance Criteria Results

| Criterion | Status | Notes |
|-----------|--------|-------|
| AC-1: std::format cache keys | Pass | All 3 formatCache_ key constructions use std::format |
| AC-2: constexpr plural rules | Pass | static constexpr std::array<LocaleRule, 32> replaces runtime unordered_map |
| AC-3: %B specifier support | Pass | case 'B' added to formatDateWithConfig switch, maps to full month name |

## Accomplishments

- Replaced manual string concatenation with `std::format` in formatNumber, formatPrice, and formatDate cache key construction
- Converted plural rule locale lookup from `static const std::unordered_map` to `static constexpr std::array` — eliminates heap allocation on first call
- Added `%B` specifier handling in formatDateWithConfig, fixing deferred issue where default `long_date = "%B %d, %Y"` pattern was broken

## Files Created/Modified

| File | Change | Purpose |
|------|--------|---------|
| `src/i18ncpp.cpp` | Modified | Added `#include <array>` and `#include <format>`; replaced 3 cache key constructions with std::format; replaced unordered_map with constexpr array in getPluralForm; added case 'B' in formatDateWithConfig |

## Decisions Made

| Decision | Rationale | Impact |
|----------|-----------|--------|
| Linear scan for plural lookup | Only 32 entries — simpler than binary search, cache-friendly, zero overhead | Keeps code simple while eliminating heap allocation |
| %B as alias for %F logic | Standard strftime uses %B for full month name; maintains compatibility | Default long_date pattern now works without reconfiguration |

## Deviations from Plan

None — plan executed exactly as written.

## Issues Encountered

None.

## Next Phase Readiness

**Ready:**
- All C++20 internal modernization complete (format, constexpr)
- %B deferred issue resolved
- Phase 4 (API Modernization) can proceed: std::span parameters, tr/trPlural overload simplification

**Concerns:**
- None

**Blockers:**
- None

---
*Phase: 03-format-constexpr, Plan: 01*
*Completed: 2026-04-09*
