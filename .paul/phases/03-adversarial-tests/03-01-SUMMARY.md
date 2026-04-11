---
phase: 03-adversarial-tests
plan: 01
subsystem: testing
tags: [gtest, exception-contract, hostile-fixtures, json-boundary, cache-invariant, aegis-p-005, f-test-003, f-app-002]

requires:
  - phase: 01-boundary-correctness
    provides: configure() atomic copy-and-swap rollback, widened json::exception boundaries
  - phase: 02-concurrency-invariants
    provides: per-file test-executable convention, cache-clear invariant test pattern
provides:
  - 5 EXPECT_THROW / contract tests for i18n::I18NError throw sites
  - 3 hostile JSON fixtures (not_an_object, malformed_json, leaf_is_int)
  - i18ncpp_hostile_input_tests CMake target
  - Test-level documentation of the flattenJson silent-skip contract
  - Test-level evidence for Phase 1's post-throw "unchanged cache" invariant (translation + format caches)
affects: [04-infra-docs, v0.5 fuzzing work]

tech-stack:
  added: []
  patterns:
    - "One-executable-per-test-file convention (carried from Phase 2)"
    - "Hostile fixtures co-located under tests/fixtures/hostile/ via FIXTURES_DIR"
    - "EXPECT_THROW asserts concrete I18NError, not std::exception base"

key-files:
  created:
    - tests/fixtures/hostile/not_an_object.json
    - tests/fixtures/hostile/malformed_json.json
    - tests/fixtures/hostile/leaf_is_int.json
    - tests/test_hostile_input.cpp
  modified:
    - tests/CMakeLists.txt

key-decisions:
  - "AC-3 flipped from 'caches cleared after throw' to 'caches unchanged after throw' — matches Phase 1's atomic rollback invariant, which the plan spec had misread"
  - "Reuse existing tests/fixtures/formats_wrong_type.json from Phase 1 (do not duplicate into hostile/)"
  - "No missing_file.json fixture — the missing-file test deliberately names a non-existent path"
  - "Zero runtime-code change — src/i18ncpp.cpp and include/i18ncpp.h untouched by this plan"

patterns-established:
  - "Throw-site tests assert the concrete i18n::I18NError type, not std::exception or std::runtime_error"
  - "Post-throw invariant tests snapshot cache sizes pre-throw and compare post-throw (unchanged assertion), rather than asserting zeroed sizes"

duration: ~30min
started: 2026-04-11T02:00:00Z
completed: 2026-04-11T02:30:00Z
---

# Phase 3 Plan 01: Adversarial Tests — Hostile-Input Fixtures Summary

**Installed 5 EXPECT_THROW / contract tests for i18n::I18NError using 3 hostile JSON fixtures, closing AEGIS P-005 and F-TEST-003 with zero runtime-code change.**

## Performance

| Metric | Value |
|--------|-------|
| Duration | ~30min |
| Started | 2026-04-11T02:00:00Z |
| Completed | 2026-04-11T02:30:00Z |
| Tasks | 3 completed |
| Files created | 4 |
| Files modified | 1 |

## Acceptance Criteria Results

| Criterion | Status | Notes |
|-----------|--------|-------|
| AC-1: Four throw sites produce I18NError | PASS | 4/4 tests green: missing file, malformed JSON, top-level array, formats_wrong_type |
| AC-2: Non-string leaves silently skipped | PASS | leaf_is_int.json loads; tr("greeting") returns "greeting" (fallback), tr("farewell") returns "bye" |
| AC-3: Post-throw cache invariant holds (P-001 coupling) | PASS (deviated) | Flipped from "cleared" to "unchanged" — see Deviations |

## Accomplishments

- F-TEST-003 structurally closed: `rg EXPECT_THROW tests/` now returns 6 matches (was 0 at F-TEST-003 discovery, 2 after Phase 1). The i18n::I18NError contract is now directly tested.
- F-APP-002 resolved at the test level: flattenJson's silent-skip behavior for non-string leaves (src/i18ncpp.cpp:202-228) is now pinned by `LeafIsIntSilentlySkipped`.
- Cache-invariant coupling extended: Phase 1's `ConfigureRollbackOnTypeError` asserted unchanged format cache only; test 5 also asserts unchanged translation cache, closing the coupling from a second angle.

## Files Created/Modified

| File | Change | Purpose |
|------|--------|---------|
| tests/fixtures/hostile/not_an_object.json | Created | Top-level JSON array — triggers json::exception inside flattenJson |
| tests/fixtures/hostile/malformed_json.json | Created | Unterminated JSON object — triggers json::parse_error |
| tests/fixtures/hostile/leaf_is_int.json | Created | Non-string leaf with sibling string — silent-skip contract fixture |
| tests/test_hostile_input.cpp | Created | 5 tests covering 4 throw sites + silent-skip contract + post-throw invariant |
| tests/CMakeLists.txt | Modified | Added i18ncpp_hostile_input_tests target and discovery registration |

## Decisions Made

| Decision | Rationale | Impact |
|----------|-----------|--------|
| AC-3 flipped to "unchanged" semantics | Plan misread Phase 1's rollback contract; Phase 1's copy-and-swap preserves state on throw, never clears it | Test 5 asserts formatCacheSize == formatCachePre (not 0). Closes P-001/P-005 coupling correctly |
| Share tests/fixtures/formats_wrong_type.json with Phase 1 | Moving it would break test_locale_mgmt.cpp:100,116 | No fixture duplication; both test files exercise the same fixture with different invariants |
| No missing_file.json fixture | Missing-file test relies on the path not existing | File absence is the contract; creating the file would invalidate the test |

## Deviations from Plan

### Summary

| Type | Count | Impact |
|------|-------|--------|
| Spec correction (auto-fixed with user approval) | 1 | Test 5 post-condition flipped; no runtime-code impact |
| Scope additions | 0 | - |
| Deferred | 0 | - |

**Total impact:** One spec-level correction caught during qualify. No scope creep. Runtime binary unchanged.

### Spec Correction: AC-3 Post-Throw Invariant

- **Found during:** Task 2/3 qualify, after running ctest against the first version of test_hostile_input.cpp
- **Issue:** Plan AC-3 asserted `formatCacheSize() == 0 && translationCacheSize() == 0` after throw. This contradicted Phase 1's `ConfigureRollbackOnTypeError` (tests/test_locale_mgmt.cpp:109) which asserts `formatCacheSize() == cachePre` (unchanged). Both cannot be true — Phase 1's `configure()` uses copy-and-swap atomic rollback, which preserves state on throw.
- **Root cause classification:** Spec issue. Plan author conflated Phase 2's success-path cache-clear invariant with Phase 1's failure-path atomic rollback.
- **Fix:** Changed test 5 to snapshot `translationCachePre` and `formatCachePre` before the throwing call, and assert equality afterward. No source-code change; no boundary violation.
- **Files:** tests/test_hostile_input.cpp (test 5 only)
- **Verification:** `ctest --test-dir build -C Debug` reports 98/98 passing after the fix.
- **User approval:** Option 1 selected from the checkpoint escalation presented during APPLY.

## Verification Results

- `ls tests/fixtures/hostile/` — exactly 3 files: leaf_is_int.json, malformed_json.json, not_an_object.json
- `cmake --build build --config Debug --target i18ncpp_hostile_input_tests` — succeeded with no new warnings
- `build/tests/Debug/i18ncpp_hostile_input_tests.exe` — built and registered
- `ctest --test-dir build -C Debug` — 98/98 passing (93 baseline + 5 new)
- `ctest --test-dir build -C Debug -R HostileInput` — 5/5 passing
- `rg EXPECT_THROW tests/` — 6 total (4 in test_hostile_input.cpp, 2 in test_locale_mgmt.cpp)
- `git diff --stat src/` — no changes (boundary respected)

## Issues Encountered

| Issue | Resolution |
|-------|------------|
| Test 5 failed first run: plan spec contradicted Phase 1 invariant | Escalated as spec issue, user selected option 1 (flip to "unchanged"), fix applied, re-verified green |

## Skill Audit

No .paul/SPECIAL-FLOWS.md configured — skill audit N/A.

## Next Phase Readiness

**Ready:**
- Exception contract now has test evidence — safe to refactor error paths knowing regressions will fail loudly
- F-TEST-003 and F-APP-002 closed; remaining AEGIS mediums tracked in Phase 4 scope
- Hostile-fixture directory established as a pattern for any future adversarial tests (v0.5 fuzzing could land alongside)

**Concerns:**
- Plan AC-3 spec bug is a reminder that post-throw invariant semantics ("unchanged" vs "cleared") need explicit classification during PLAN — worth flagging in any future plan touching configure()/loadLocale rollback assertions

**Blockers:**
- None

---
*Phase: 03-adversarial-tests, Plan: 01*
*Completed: 2026-04-11*
