---
phase: 01-boundary-correctness
plan: 01
subsystem: api
tags: [exception-safety, filesystem, nlohmann-json, copy-and-swap, regression-tests]

requires:
  - phase: milestone-v0.4
    provides: stable FormatConfig aggregates, flat localesData storage, format/translation caches

provides:
  - configure() strong exception safety (copy-and-swap)
  - unified json::exception translation to I18NError across loadLocale/mergeLocale/load/configure
  - std::filesystem::path routing for ifstream opens (non-null-terminated view safe, Windows Unicode safe)
  - hostile fixture formats_wrong_type.json (reused by Phase 3 P-005)
  - 3 regression tests covering AC-1/AC-2/AC-3

affects: [phase-03-adversarial-tests, concurrency-phase-02, documentation-phase-04]

tech-stack:
  added: [std::filesystem]
  patterns:
    - "Copy-and-swap at JSON trust boundary — stage into local tmp, translate json::exception, nothrow commit"
    - "std::filesystem::path at ifstream construction site — handles non-null-terminated string_view and platform path encoding"

key-files:
  created:
    - tests/fixtures/formats_wrong_type.json
  modified:
    - src/i18ncpp.cpp
    - tests/test_locale_mgmt.cpp

key-decisions:
  - "configure() wraps only json::exception; std::bad_alloc and non-JSON errors still propagate unchanged"
  - "load(const json&) clears caches on throw but does not roll back partial localesData merges (deferred to v0.5 per P-001 Change 4)"
  - "Used brace-init std::ifstream{std::filesystem::path(filePath)} to avoid most-vexing parse under MSVC"
  - "No CMakeLists.txt change for fixture: FIXTURES_DIR compile definition already points at source dir"

patterns-established:
  - "JSON trust boundary discipline: any public method that walks nlohmann/json must catch json::exception and rethrow as I18NError"
  - "Filesystem trust boundary discipline: any std::ifstream opened from a std::string_view path must go through std::filesystem::path"

duration: ~30min
started: 2026-04-11T00:00:00Z
completed: 2026-04-11T00:30:00Z
---

# Phase 1 Plan 01: Boundary Correctness Summary

**AEGIS M1/M2 closed: configure() is now transactional (copy-and-swap with nothrow commit), all JSON boundary catches widened to json::exception, and loadLocale/mergeLocale open files via std::filesystem::path — fixing latent UB on non-null-terminated string_view and silent Windows Unicode-path failure.**

## Performance

| Metric | Value |
|--------|-------|
| Duration | ~30 min |
| Started | 2026-04-11T00:00:00Z |
| Completed | 2026-04-11T00:30:00Z |
| Tasks | 3 completed (3 of 3) |
| Files modified | 3 (1 created, 2 modified) |

## Acceptance Criteria Results

| Criterion | Status | Notes |
|-----------|--------|-------|
| AC-1: configure() provides strong exception safety | PASS | `ConfigureRollbackOnTypeError` verifies I18NError throw, unchanged currency.symbol, unchanged formatCacheSize after hostile array-for-string fixture |
| AC-2: loadLocale / mergeLocale / load(json) surface only I18NError | PASS | `LoadLocaleCatchesJsonTypeError` verifies a type_error thrown inside configure() inside loadLocale() is re-translated to I18NError at the outer boundary. Catch clauses widened from json::parse_error to json::exception in all three entry points |
| AC-3: loadLocale accepts non-null-terminated string_view | PASS | `LoadLocaleAcceptsNonNullTerminatedView` constructs a view over a backing buffer with trailing garbage and confirms the fixture loads successfully |

## Accomplishments

- **configure() transactional rewrite:** entire body stages into `FormatConfig tmp = defaultConfig`, wrapped in a try over json::exception; on success commits with `defaultConfig = std::move(tmp)` followed by cache clears as the nothrow commit window. Partial mutation on type errors is eliminated.
- **JSON boundary unified:** loadLocale (:58), mergeLocale (:106), and load(const json&) (:146) all catch `const json::exception&`, preserving the library's `I18NError`-only public exception contract under adversarial JSON input.
- **Filesystem boundary fixed:** `std::ifstream{std::filesystem::path(filePath)}` at loadLocale:39 and mergeLocale:84 — the path object copies the full view range and handles platform-native encoding, removing both the non-null-terminated string_view UB and the silent Windows Unicode-path failure.
- **Hostile fixture installed:** `tests/fixtures/formats_wrong_type.json` with `_formats.currency.symbol` as an array — reusable by Phase 3 P-005 adversarial suite, no duplication needed later.
- **Regression tests:** 3 new GTest cases attached to the existing LocaleMgmtTest fixture — 85/85 tests green (82 pre-existing + 3 new).

## Task Commits

Not yet committed as atomic task commits — all three tasks applied in one working-tree state, commit will happen at transition via the phase commit. User prefers `oco --yes` for commit messages (per memory rules).

| Task | Type | Description |
|------|------|-------------|
| Task 1: Rewrite configure() + widen JSON catches | refactor | Copy-and-swap configure(), widen catches to json::exception in loadLocale/mergeLocale/load |
| Task 2: Route file opens through std::filesystem::path | fix | Add `#include <filesystem>`, replace `ifstream(filePath.data())` with `ifstream{std::filesystem::path(filePath)}` |
| Task 3: Hostile fixture + 3 regression tests | test | Create formats_wrong_type.json, add ConfigureRollbackOnTypeError / LoadLocaleCatchesJsonTypeError / LoadLocaleAcceptsNonNullTerminatedView |

## Files Created/Modified

| File | Change | Purpose |
|------|--------|---------|
| `src/i18ncpp.cpp` | Modified | configure() copy-and-swap + json::exception catches + std::filesystem::path routing + `#include <filesystem>` |
| `tests/test_locale_mgmt.cpp` | Modified | 3 new TEST_F cases; added `<fstream>` and `<string_view>` includes |
| `tests/fixtures/formats_wrong_type.json` | Created | Hostile fixture: `_formats.currency.symbol` as array (wrong type) — triggers json::type_error mid-configure |

## Decisions Made

| Decision | Rationale | Impact |
|----------|-----------|--------|
| Catch only `json::exception`, not all `std::exception` | Explicitly scoped to translating the nlohmann/json trust boundary. `std::bad_alloc` must still propagate cleanly. | configure() is exception-safe for JSON type errors but remains strict about allocation failures |
| `load(const json&)` clears caches on throw but does NOT roll back `localesData` | Per P-001 Change 4 unremediated-consequences note — full rollback of partially-merged locales is deferred to v0.5 | Cache invariant is preserved (caches flushed on throw), but already-merged locales from earlier iterations of the loop remain in `localesData`. Documented as known limitation. |
| Brace-init `std::ifstream{...}` instead of paren-init | MSVC parses `std::ifstream fs(std::filesystem::path(filePath))` as a function declaration (most-vexing parse). Discovered at build time. | Both loadLocale and mergeLocale use `{}` for the ifstream construction |
| No tests/CMakeLists.txt change | FIXTURES_DIR compile definition already points at `${CMAKE_CURRENT_SOURCE_DIR}/fixtures`, no per-file glob or copy rule | New fixture picked up automatically — plan conditional ("if not already globbed") was the correct guard |
| Before-fix sanity check (revert Task 1, confirm new test fails, re-apply) not performed | Plan marked this optional. Skipped to avoid churning the edit in/out — build-and-pass + spec inspection was sufficient evidence. | Logged as a minor deviation for transparency; ACs still verified by code review + passing tests |

## Deviations from Plan

### Summary

| Type | Count | Impact |
|------|-------|--------|
| Auto-fixed | 1 | Most-vexing parse fix at build time — no scope change |
| Scope additions | 0 | None |
| Deferred | 0 | None |

**Total impact:** All deviations were mechanical and discovered at build time. No scope creep, no ACs weakened.

### Auto-fixed Issues

**1. [build] Most-vexing parse on std::ifstream constructor with filesystem::path**
- **Found during:** Task 2 verify step (`cmake --build build --config Debug`)
- **Issue:** `std::ifstream fileStream(std::filesystem::path(filePath));` is parsed by MSVC as a function declaration `fileStream` taking `filePath` of type `std::filesystem::path`. Next-line `fileStream.is_open()` then produced C2228: "left of '.is_open' must have class/struct/union".
- **Fix:** Replaced paren-init with brace-init: `std::ifstream fileStream{std::filesystem::path(filePath)};` at both loadLocale:39 and mergeLocale:84.
- **Files:** `src/i18ncpp.cpp`
- **Verification:** Rebuilt clean, 85/85 tests pass.
- **Commit:** will be part of the phase commit

### Skipped Optional Verification

**Before-fix sanity check** (Task 3 optional verify): plan suggested temporarily reverting Task 1 to confirm `ConfigureRollbackOnTypeError` actually exercises the bug. Not performed. Mitigation: the test was designed against the pre-fix behavior (array-to-string conversion throws json::type_error), and the AC-1 observable checks (symbol + cache size) would only hold if the rollback is real. Low risk.

### Deferred Items

None.

## Issues Encountered

| Issue | Resolution |
|-------|------------|
| MSVC most-vexing parse on ifstream+path (Task 2) | Switched to brace-init; one-line fix at each call site |

## Next Phase Readiness

**Ready:**
- JSON trust boundary is unified and exception-safe — Phase 2 (concurrency & invariants) can assume `I18NError` is the only thing crossing the public API under adversarial JSON
- Filesystem trust boundary is unified — Phase 4 (infra & docs) can document the non-null-terminated / Unicode-path guarantees in README/BASELINE without caveats
- `tests/fixtures/formats_wrong_type.json` is in place and reusable — Phase 3 P-005 can add more type-error variants next to it without re-creating the copy infrastructure
- 85 passing tests as a regression baseline for all subsequent phases

**Concerns:**
- `load(const json&)` preserves cache invariant but not localesData invariant on partial-merge failure — should be revisited in v0.5 with a proper transaction scope
- Windows Unicode-path test deliberately not included (AC-3 Japanese fixture optional per P-002); any future Windows-specific regression will require adding that fixture
- AEGIS P-003 (thread-safety contract) and P-004 (cache-invariant parametrized test) remain open — queued for Phase 2

**Blockers:** None.

---
*Phase: 01-boundary-correctness, Plan: 01*
*Completed: 2026-04-11*
