# Roadmap: i18ncpp

## Overview

Build a lightweight, dependency-minimal C++ i18n library that provides translation, pluralization, and locale-aware formatting — evolving from initial implementation toward a tested, documented, production-ready library.

## Current Milestone

**v0.4.2 Correctness & Portability** — Status: In Progress
Phases: 1 of 2 complete
Focus: Close all findings from REVIEW-2026-04-11 (5 bugs + 4 coverage gaps) and make the library buildable on GCC/Clang toolchains. No new features. No CI.

| Phase | Name | Plans | Status | Completed |
|-------|------|-------|--------|-----------|
| 1 | Correctness | 1 (01-01) | ✅ Complete | 2026-04-13 |
| 2 | Portability | TBD | Not started | — |

### Phase 1: Correctness

Focus: F1 (load replace + atomic rollback), F2 (format config isolation via `baselineConfig_`), F3 (bool branch order in `toString`), F4 (keyExists parity with tr/trPlural) + regression tests G1–G4.
Plans: TBD (defined during /paul:plan)

### Phase 2: Portability

Focus: F5 — `localtime_r`/`localtime_s` shim, decide `std::format` floor (GCC 13+/Clang 17+) vs fallback, document toolchain requirements in README + CLAUDE.md.
Plans: TBD (defined during /paul:plan)

## Completed Milestones

<details>
<summary>v0.4.1 Hardening & Infrastructure - 2026-04-11 (4 phases)</summary>

Focus: Closed AEGIS medium findings and installed operational infrastructure (CI/CD, docs) before the next feature cycle.

| Phase | Name | Plans | Completed |
|-------|------|-------|-----------|
| 1 | Boundary Correctness | 1 (01-01) | 2026-04-11 |
| 2 | Concurrency & Invariants | 1 (02-01) | 2026-04-11 |
| 3 | Adversarial Tests | 1 (03-01) | 2026-04-11 |
| 4 | Infra & Docs | 2 (04-01, 04-02) | 2026-04-11 |

Deliverables:
- P-001: `configure()` copy-and-swap transactional commit (nothrow window)
- P-002: `std::filesystem::path` used for `ifstream` to handle Unicode paths + non-null-terminated `string_view`
- P-003: Thread-safety contract documented at class boundary (`\warning` docblock in `include/i18ncpp.h`), TSan regression test gated by `-DI18N_ENABLE_TSAN=ON`
- P-004: Cache-clear invariant enforced by parametrized test enumerating all 8 public mutators (`tests/test_cache_invariant.cpp`)
- P-005: Hostile-input fixtures (malformed JSON, non-object root, non-string leaves) + regression tests (`tests/test_hostile_input.cpp`, `tests/fixtures/hostile/`)
- TSK-004: `CLAUDE.md` project agent-onboarding doc
- TSK-005: `.github/workflows/ci.yml` matrix CI (Ubuntu + Windows + macOS, Release, Conan-driven) + README badge
- TSK-006: `README.md` refreshed for C++20 + current `tr`/`trPlural` API surface, `benchmarks/BASELINE.md` header + v0.4.0 status section

</details>


<details>
<summary>v0.4 C++20 Modernization - 2026-04-09 (4 phases)</summary>

| Phase | Name | Plans | Completed |
|-------|------|-------|-----------|
| 1 | C++20 Migration | 1/1 | 2026-04-09 |
| 2 | Transparent Hash & Zero-Copy | 1/1 | 2026-04-09 |
| 3 | Format & Constexpr | 1/1 | 2026-04-09 |
| 4 | API Modernization | 1/1 | 2026-04-09 |

Archive: `.paul/milestones/v0.4.0-ROADMAP.md`

</details>

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
*Roadmap updated: 2026-04-13 — Phase 1 (Correctness) complete*
