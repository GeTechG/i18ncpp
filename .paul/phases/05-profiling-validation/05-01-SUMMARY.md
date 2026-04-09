---
phase: 05-profiling-validation
plan: 01
subsystem: testing
tags: [benchmarks, profiling, performance-validation, comparison]

requires:
  - phase: 01-benchmarks
    provides: baseline benchmark data and harness
  - phase: 04-caching-compile-time
    provides: final optimization (translation cache)
provides:
  - Before/after performance comparison for all 18 benchmarks
  - Validated optimization results for v0.3 milestone
affects: []

tech-stack:
  added: []
  patterns: []

key-files:
  created: []
  modified: [benchmarks/BASELINE.md]

key-decisions:
  - "Averaged 2 runs per benchmark for stability (harness lacks stddev)"
  - "InterpolateNamed regex bottleneck documented as remaining hot spot — out of scope for v0.3"

patterns-established: []

duration: 5min
completed: 2026-04-09T00:00:00Z
---

# Phase 5 Plan 01: Profiling & Validation Summary

**Re-ran all 18 benchmarks post-optimization: lookup 87-96% faster, positional interpolation 94-96% faster, named interpolation unchanged (regex bottleneck documented)**

## Performance

| Metric | Value |
|--------|-------|
| Duration | ~5min |
| Completed | 2026-04-09 |
| Tasks | 2 completed |
| Files modified | 1 |

## Acceptance Criteria Results

| Criterion | Status | Notes |
|-----------|--------|-------|
| AC-1: Benchmarks build and run | Pass | All 3 executables built Release mode, 18 benchmarks ran successfully |
| AC-2: Before/after comparison documented | Pass | BASELINE.md updated with baseline, current, comparison table, and analysis |
| AC-3: Key optimization targets analyzed | Pass | Major wins and remaining hot spot (InterpolateNamed) documented |

## Accomplishments

- Validated v0.3 optimization work with concrete before/after data across all 18 benchmarks
- Documented massive gains: TrSimpleKey 205→22 ns/op (89%), InterpolatePositional 1545→65 ns/op (96%), TrPlural 1535→56 ns/op (96%)
- Identified InterpolateNamed (6740 ns/op, regex-bound) as the sole remaining hot spot for future work

## Files Created/Modified

| File | Change | Purpose |
|------|--------|---------|
| `benchmarks/BASELINE.md` | Modified | Added v0.3.0 results, comparison tables, and analysis |

## Key Results

| Category | Benchmark | Baseline | Current | Change |
|----------|-----------|----------|---------|--------|
| Lookup | TrSimpleKey | 205 ns | 22 ns | **-89%** |
| Lookup | TrPlural | 1535 ns | 56 ns | **-96%** |
| Lookup | TrMissingFallback | 315 ns | 22 ns | **-93%** |
| Interpolation | InterpolatePositional | 1545 ns | 65 ns | **-96%** |
| Interpolation | InterpolateTwoParams | 2224 ns | 125 ns | **-94%** |
| Interpolation | InterpolateNamed | 6775 ns | 6740 ns | -1% |
| Interpolation | PluralWithParam | 2266 ns | 99 ns | **-96%** |
| Formatting | FormatNumber | 338 ns | 342 ns | +1% |

## Decisions Made

| Decision | Rationale | Impact |
|----------|-----------|--------|
| Average of 2 runs | Harness lacks stddev; 2 runs showed consistent results | Sufficient for relative comparison |
| Document regex as remaining hot spot | InterpolateNamed unchanged — std::regex dominates | Future milestone could replace regex with custom parser |

## Deviations from Plan

None — plan executed exactly as written.

## Next Phase Readiness

**Ready:**
- v0.3 milestone complete — all 5 phases done
- Performance data validates optimization investment
- Clear documentation of what improved and what remains

**Concerns:**
- InterpolateNamed (6740 ns/op) remains regex-bound — future work could replace std::regex with a custom `%{key}` parser for ~100x improvement
- Benchmark harness lacks stddev — acceptable for relative comparison but not for absolute claims

**Blockers:**
- None

---
*Phase: 05-profiling-validation, Plan: 01*
*Completed: 2026-04-09*
