# Roadmap: i18ncpp

## Overview

Build a lightweight, dependency-minimal C++ i18n library that provides translation, pluralization, and locale-aware formatting — evolving from initial implementation toward a tested, documented, production-ready library.

## Current Milestone

**v0.4 C++20 Modernization**
Status: In Progress
Phases: 1 of 4 complete

| Phase | Name | Plans | Status | Completed |
|-------|------|-------|--------|-----------|
| 1 | C++20 Migration | 1/1 | ✅ Complete | 2026-04-09 |
| 2 | Transparent Hash & Zero-Copy | TBD | Not started | - |
| 3 | Format & Constexpr | TBD | Not started | - |
| 4 | API Modernization | TBD | Not started | - |

### Phase 1: C++20 Migration

Focus: Switch CMake/Conan build configuration to C++20, verify compilation, fix compatibility issues

### Phase 2: Transparent Hash & Zero-Copy

Focus: Heterogeneous lookup for all unordered_maps, string_view key lookup — zero-allocation lookups

### Phase 3: Format & Constexpr

Focus: std::format for cache keys, constexpr plural rules, fix %B specifier in formatDateWithConfig

### Phase 4: API Modernization

Focus: std::span parameters, simplify tr/trPlural overloads, cleanup

## Completed Milestones

<details>
<summary>v0.3 Deep Performance - 2026-04-09 (6 phases)</summary>

| Phase | Name | Plans | Completed |
|-------|------|-------|-----------|
| 1 | Benchmarks | 1 (01-01) | 2026-04-09 |
| 2 | Zero-copy & Move Semantics | 1 (02-01) | 2026-04-09 |
| 3 | Allocation Optimization | 1 (03-01) | 2026-04-09 |
| 4 | Caching & Compile-time | 1 (04-01) | 2026-04-09 |
| 5 | Profiling & Validation | 1 (05-01) | 2026-04-09 |
| 6 | Regex Replacement | 1 (06-01) | 2026-04-09 |

Archive: `.paul/milestones/v0.3.0-ROADMAP.md`

</details>

<details>
<summary>v0.2 Performance & Reliability (3 phases)</summary>

| Phase | Name | Status |
|-------|------|--------|
| 1 | Tests | Complete |
| 2 | Flat Storage | Complete |
| 3 | Formatting Cache | Complete |

</details>

<details>
<summary>v0.1 Initial Release</summary>

Shipped — initial implementation.

</details>

---
*Roadmap updated: 2026-04-09 — Phase 1 (C++20 Migration) complete*
