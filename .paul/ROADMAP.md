# Roadmap: i18ncpp

## Overview

Build a lightweight, dependency-minimal C++ i18n library that provides translation, pluralization, and locale-aware formatting — evolving from initial implementation toward a tested, documented, production-ready library.

## Current Milestone

**v0.3 Deep Performance** (v0.3.0)
Status: In Progress
Phases: 3 of 5 complete

## Phases

| Phase | Name | Plans | Status | Completed |
|-------|------|-------|--------|-----------|
| 1 | Benchmarks | 1 (01-01-benchmarks) | Complete | 2026-04-09 |
| 2 | Zero-copy & Move Semantics | 1 (02-01) | Complete | 2026-04-09 |
| 3 | Allocation Optimization | 1 (03-01) | Complete | 2026-04-09 |
| 4 | Caching & Compile-time | TBD | Not started | - |
| 5 | Profiling & Validation | TBD | Not started | - |

## Phase Details

### Phase 1: Benchmarks

Focus: Baseline benchmark suite for hot paths (lookup, interpolation, formatting). Measure before optimizing.
Plans: TBD (defined during /paul:plan)

### Phase 2: Zero-copy & Move Semantics

Focus: Use `std::string_view` for key lookup and interpolation to eliminate unnecessary string copies. Optimize data transfer with move semantics for JSON parsing, locale merge, and internal structures.
Plans: TBD (defined during /paul:plan)

### Phase 3: Allocation Optimization

Focus: Pre-allocated memory pooling for interpolation buffers. Leverage SSO-friendly patterns for short translation strings to avoid heap allocations.
Plans: TBD (defined during /paul:plan)

### Phase 4: Caching & Compile-time

Focus: Cache interpolation results with appropriate invalidation. Constexpr hashing for statically known translation keys.
Plans: TBD (defined during /paul:plan)

### Phase 5: Profiling & Validation

Focus: Re-run benchmarks, compare with Phase 1 baseline, identify and optimize remaining hot spots.
Plans: TBD (defined during /paul:plan)

## Completed Milestones

### v0.2 Performance & Reliability (v0.2.0)
Status: Complete
Phases: 3 of 3 complete
- Phase 1: Tests (complete)
- Phase 2: Flat Storage (complete)
- Phase 3: Formatting Cache (complete)

### v0.1 Initial Release (v0.1.0)
Status: Complete (shipped)

---
*Roadmap updated: 2026-04-09 — Phase 3 (Allocation Optimization) complete*
