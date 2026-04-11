---
phase: 04-infra-docs
plan: 01
subsystem: docs
tags: [claude-md, readme, baseline, cpp20, documentation, api-surface]

requires:
  - phase: 01-boundary-correctness
    provides: configure() copy-and-swap, JSON boundary hardening (described in CLAUDE.md conventions)
  - phase: 02-concurrency-invariants
    provides: thread-safety contract, cache-clear invariant, one-exe-per-test-file convention
  - phase: 03-adversarial-tests
    provides: hostile-input coverage (informs "no deferred issues" framing in docs)
provides:
  - Project-level CLAUDE.md for AI agent onboarding (build, test, layout, constraints, conventions)
  - README.md aligned with v0.4 C++20 API surface (span / initializer_list / variadic tr overloads)
  - benchmarks/BASELINE.md with corrected build header and explicit v0.4.0 status section
affects: [04-02-cicd, next-milestone-planning, new-contributor-onboarding]

tech-stack:
  added: []
  patterns:
    - "Single-source build/test/layout docs: CLAUDE.md points at .paul/ for state, never duplicates it"
    - "BASELINE.md append-only convention: v0.4.0 Status section preserves historical v0.2/v0.3 tables unchanged"

key-files:
  created:
    - CLAUDE.md
  modified:
    - README.md
    - benchmarks/BASELINE.md

key-decisions:
  - "v0.4.0 status in BASELINE.md documented as 'no re-measurement required' rather than rerunning benchmarks, because v0.4 did not target hot paths"
  - "Individual-param tr overloads explicitly flagged as removed in both CLAUDE.md and README — prevents future AI sessions or contributors from reintroducing them"

patterns-established:
  - "CLAUDE.md is the single agent-onboarding surface — any new build/test/layout convention added in future phases must be reflected there"
  - "BASELINE.md grows by appending dated status sections; historical tables are never edited"

duration: ~20min
started: 2026-04-11
completed: 2026-04-11
---

# Phase 4 Plan 01: Docs Refresh (CLAUDE.md + README + BASELINE) Summary

**Documentation surface brought current with v0.4 reality: new project CLAUDE.md, README updated to C++20 + current tr/trPlural API, BASELINE.md header fixed and v0.4 status appended.**

## Performance

| Metric | Value |
|--------|-------|
| Duration | ~20 min |
| Tasks | 3 of 3 completed |
| Files created | 1 (CLAUDE.md) |
| Files modified | 2 (README.md, benchmarks/BASELINE.md) |
| Checkpoints | 0 (autonomous plan) |
| Qualify attempts | 3 (all first-pass PASS) |

## Acceptance Criteria Results

| Criterion | Status | Notes |
|-----------|--------|-------|
| AC-1: CLAUDE.md actionable for cold AI session | **Pass** | Build/test commands, layout, C++20 + nlohmann-only + no-ICU + no-regex constraints, single-writer thread contract, pointer to `.paul/` state. 2 `C++20` matches, 3 `nlohmann/json` matches, 6 build/test command matches. |
| AC-2: README matches current API and language standard | **Pass** | `C++17` count = 0; `C++20` count = 1; Translation Methods section rewritten to span / initializer_list / variadic form; every listed signature verified against `include/i18ncpp.h`; Basic Usage snippet updated (`trv("welcome", "John")` + `tr("welcome", {"John"})`) — no removed-overload references remain. |
| AC-3: BASELINE.md header accurate and v0.4 status explicit | **Pass** | Header `C++17` → `C++20`; `## v0.4.0 Status` section appended between v0.3.0 Post-Regex-Removal and Before/After Comparison, explicitly documenting that v0.4 did not target benchmarked hot paths; v0.2.0 / v0.3.0 Post-Optimization / v0.3.0 Post-Regex-Removal historical sections intact per section-header grep. |

## Accomplishments

- **Cold-session AI onboarding is now possible without reading `.paul/` or grepping the codebase.** CLAUDE.md (~95 lines) gives build commands, test commands, layout, hard constraints, conventions, and the single-public-header entrypoint.
- **README no longer advertises removed API.** The v0.4 Phase 4 overload-set simplification (span / initializer_list / variadic replacing individual-param overloads) is finally reflected in user-facing docs — previously a reader following the README would have hit compile errors on the "Basic Usage" snippet.
- **BASELINE.md stops lying about the build.** Header now says C++20 (as it has actually been since v0.4 Phase 1), and v0.4 is explicitly accounted for without polluting historical v0.2 / v0.3 tables.

## Task Commits

No per-task commits — this is a docs-only plan. Commits will be bundled at the plan level via `oco --yes` after UNIFY is fully closed and the user triggers it.

| Task | Status | Qualify | Files |
|------|--------|---------|-------|
| Task 1: Author CLAUDE.md | DONE | PASS | CLAUDE.md |
| Task 2: Refresh README.md | DONE | PASS | README.md |
| Task 3: Refresh BASELINE.md | DONE | PASS | benchmarks/BASELINE.md |

## Files Created/Modified

| File | Change | Purpose |
|------|--------|---------|
| `CLAUDE.md` | Created | Agent onboarding: build, test, layout, hard constraints, conventions, API entrypoint |
| `README.md` | Modified | C++20 dependency line + rewritten Translation Methods section + updated Basic Usage snippet (removed-overload references gone) |
| `benchmarks/BASELINE.md` | Modified | Build header C++17 → C++20; appended `## v0.4.0 Status` section with local validation command |

## Decisions Made

| Decision | Rationale | Impact |
|----------|-----------|--------|
| Document v0.4 as "no fresh capture required" rather than rerunning benchmarks | v0.4 changes (transparent hashing, `std::format` keys, constexpr plural rules, `std::span` API) don't touch benchmarked hot paths; the translation cache and interpolation fast paths are unchanged | Faster close of phase 4, honest numbers, and a documented local command for anyone who wants to validate |
| Explicitly call out removed individual-param `tr` overloads in README and CLAUDE.md | These overloads were a prominent part of the v0.3 README and were silently removed in v0.4 Phase 4; without an explicit "removed in v0.4" callout, future contributors or AI sessions would likely reintroduce them | Locks the API simplification in at the docs layer |

## Deviations from Plan

### Summary

| Type | Count | Impact |
|------|-------|--------|
| Auto-fixed | 0 | — |
| Scope additions | 0 | — |
| Deferred | 0 | — |

**Total impact:** None. Plan executed exactly as written, first-pass PASS on all three qualify cycles.

### Deferred Items

None.

## Issues Encountered

None during execution. One ambient observation (not a plan issue, not scope-creep): the working tree has pre-existing modifications from earlier phases (`M include/i18ncpp.h`, `M tests/CMakeLists.txt`, `M .paul/ROADMAP.md`, `M .paul/paul.json`, `M .gitignore`, plus untracked phase 2/3 fixture/test files). These predate this APPLY and are outside the plan's boundary. They should be reconciled before phase 04-02 (CI/CD) so that the first CI run starts from a clean tree — flagging here so it is not forgotten.

## Next Phase Readiness

**Ready:**
- Documentation surface is current, so plan 04-02 (CI/CD — GitHub Actions) can reference README / CLAUDE.md build+test commands verbatim rather than re-deriving them.
- Hard constraints are now documented in one place (CLAUDE.md), giving 04-02 a canonical source for the "what must the CI matrix cover" question (C++20, MSVC multi-config, BUILD_TESTS=ON, ctest `-C Release`).

**Concerns:**
- Pre-existing dirty working tree (see Issues Encountered) must be reconciled before CI/CD lands — otherwise the first Actions run will be testing stale/unreviewed state from phases 1-3.
- No changelog/CHANGELOG.md exists; v0.4 → v0.4.1 narrative currently only lives in `.paul/` phase summaries. Not in scope for phase 4 but worth noting.

**Blockers:**
- None for plan 04-02 (CI/CD). The dirty-tree issue is a pre-plan hygiene step, not a structural blocker.

---
*Phase: 04-infra-docs, Plan: 01*
*Completed: 2026-04-11*
