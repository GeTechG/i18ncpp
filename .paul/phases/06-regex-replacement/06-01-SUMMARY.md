---
phase: 06-regex-replacement
plan: 01
subsystem: core
tags: [regex, interpolation, performance, manual-parser]

requires:
  - phase: 05-profiling-validation
    provides: benchmark data identifying regex as remaining bottleneck
provides:
  - Manual character-by-character parsers replacing all 4 std::regex patterns
  - Complete removal of <regex> header dependency
  - InterpolateNamed performance from ~6740 ns/op to ~489 ns/op
affects: []

tech-stack:
  added: []
  patterns: [manual string scanning for pattern matching]

key-files:
  modified: [include/i18ncpp.h, src/i18ncpp.cpp, benchmarks/BASELINE.md]

key-decisions:
  - "Skip second interpolation pass (%<key>.fmt) when text has no %< pattern"
  - "Reuse local key buffer across loop iterations to avoid repeated heap allocations"
  - "Work on string_view directly in first pass to avoid initial string copy"

patterns-established:
  - "Manual parser pattern: scan for delimiter, validate key chars, extract and replace"

duration: ~15min
completed: 2026-04-09T12:00:00Z
---

# Phase 6 Plan 1: Regex Replacement Summary

**Replaced all 4 std::regex patterns with hand-written parsers, reducing InterpolateNamed from 6740 to 489 ns/op (93% faster) and fully removing the `<regex>` dependency.**

## Performance

| Metric | Value |
|--------|-------|
| Tasks | 2 completed |
| Files modified | 3 |

## Acceptance Criteria Results

| Criterion | Status | Notes |
|-----------|--------|-------|
| AC-1: All existing tests pass | Pass | 82/82 tests pass |
| AC-2: Named interpolation under 500 ns/op (>90% improvement) | Pass | 489 ns/op avg, 93% improvement from 6740 baseline |
| AC-3: std::regex dependency removed | Pass | No #include <regex>, no std::regex members remain |

## Accomplishments

- Replaced `fieldPattern` regex (`%{key}`) with manual `%{` scan + alphanumeric/underscore/dot key validation
- Replaced `valuePattern` regex (`%<key>.fmt`) with manual `%<` scan + format char extraction
- Replaced `indexPattern` regex (`{N}`) with manual `{` scan + digit parsing (no stoi)
- Replaced `emptyBracePattern` regex (`{}`) with simple two-char sequence scan
- Removed 4 static `std::regex` member declarations and definitions
- Removed `#include <regex>` from both header and source

## Files Created/Modified

| File | Change | Purpose |
|------|--------|---------|
| `include/i18ncpp.h` | Modified | Removed `#include <regex>`, removed 4 static regex member declarations |
| `src/i18ncpp.cpp` | Modified | Replaced regex-based interpolation with manual parsers, removed regex definitions, added early-exit for %< pass, optimized translate() vector copy |
| `benchmarks/BASELINE.md` | Modified | Added v0.3.0 Post-Regex-Removal section with final benchmark results |

## Deviations from Plan

### Summary

| Type | Count | Impact |
|------|-------|--------|
| Scope additions | 1 | Minor — no behavior change |

**Total impact:** Essential optimization to meet 500 ns/op target, no scope creep.

### Scope Addition

**1. translate() vector copy optimization**
- **Found during:** Task 2 (benchmarking)
- **Issue:** InterpolateNamed was ~555 ns/op after regex removal — above 500 ns/op target. The `translate()` function copied the `locales` vector every call even when not needed.
- **Fix:** Defer vector copy until `params` contains a `"locale"` key (uncommon case). Use pointer to original vector in common path.
- **Verification:** Benchmark dropped to ~489 ns/op, consistently under 500.

## Decisions Made

| Decision | Rationale | Impact |
|----------|-----------|--------|
| Skip %< pass via find() check | Most translations use %{key} only, not %<key>.fmt — avoids entire second scan loop | ~30 ns savings per call |
| Reuse local keyStr buffer | Avoids repeated heap allocation when multiple placeholders exist | Minor savings, good practice |
| Work on string_view in first pass | Eliminates initial string copy of input text | ~30 ns savings |

## Issues Encountered

None.

## Next Phase Readiness

**Ready:**
- All v0.3 Deep Performance phases (1-6) complete
- All benchmarks show 88-96% improvement across the board
- No remaining hot spots — InterpolateNamed was the last one

**Concerns:**
- None

**Blockers:**
- None

---
*Phase: 06-regex-replacement, Plan: 01*
*Completed: 2026-04-09*
