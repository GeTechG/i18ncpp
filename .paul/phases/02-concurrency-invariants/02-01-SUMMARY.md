---
phase: 02-concurrency-invariants
plan: 01
subsystem: testing
tags: [cache-invariant, thread-safety, tsan, parametrized-test, docblock, mutable-const]

requires:
  - phase: 01-boundary-correctness
    provides: unified json::exception boundary (needed by load_json mutator case to surface I18NError deterministically)
  - phase: milestone-v0.4
    provides: formatCacheSize()/translationCacheSize() accessors, mutable cache members, flat storage

provides:
  - Parametrized cache-invariant test across all 8 public mutators (AEGIS P-004 closed)
  - Class-level `I18N` thread-safety contract docblock + mutable-member cross-reference (AEGIS P-003 Changes 1+2 closed)
  - Opt-in TSan regression test target gated behind `I18N_ENABLE_TSAN` + `NOT MSVC` (AEGIS P-003 Change 3 closed on config; runtime verification deferred to Linux CI / TSK-005)

affects: [phase-03-adversarial-tests, phase-04-infra-docs, milestone-v0.4.1]

tech-stack:
  added: []
  patterns:
    - "Parametrized mutator enumeration — every public method that can mutate cached state is a test parameter; adding a new mutator without adding a `MutatorCase` is a visible review gap"
    - "Thread-safety contract documentation at type boundary — class-level `\\warning` docblock rather than per-method, because thread-safety is a per-instance property"
    - "Opt-in sanitizer targets via CMake option — `I18N_ENABLE_TSAN` default OFF, gated `NOT MSVC`, separate executable so instrumentation flags never contaminate the normal build"

key-files:
  created:
    - tests/test_cache_invariant.cpp
    - tests/test_thread_safety.cpp
  modified:
    - include/i18ncpp.h
    - tests/CMakeLists.txt

key-decisions:
  - "Tests as the enforcement layer for cache-clear invariant (D-004 resolution: no generation counter in v0.4.x)"
  - "Document the thread-safety precondition rather than add internal synchronization (P-003 resolution 1, not 2/3)"
  - "Configure mutator uses `\"EUR\"` instead of `\"€\"` — MSVC non-ASCII source-encoding hazard; cache-invalidation assertion is indifferent to the symbol value"
  - "New test file gets its own executable (project convention) despite PLAN.md Task 1 wording suggesting shared target — plan assumption about tests/CMakeLists.txt layout was wrong"
  - "TSan target is separate executable with per-target `-fsanitize=thread` compile+link options so the default MSVC build is binary-identical regardless of whether this plan landed"

patterns-established:
  - "Any new I18N mutator should add a `MutatorCase` to test_cache_invariant.cpp in the same PR — enumeration in source is the contract"
  - "Any new `mutable` member on I18N must preserve the class-level thread-safety warning — adding a new `mutable` without updating the docblock silently narrows the contract"
  - "Sanitizer-instrumented tests live in their own CMake-option-gated executable, never in the shared `i18ncpp_*_tests` set"

duration: ~25min
started: 2026-04-11T01:00:00Z
completed: 2026-04-11T01:25:00Z
---

# Phase 2 Plan 01: Concurrency & Invariants Summary

**AEGIS P-004 + P-003 both closed in one plan: parametrized cache-invariant test over 8 public mutators (85 → 93 tests), class-level thread-safety docblock on `I18N` with mutable-member cross-reference, and opt-in `I18N_ENABLE_TSAN` target for the "one instance per thread" regression guard on non-MSVC toolchains.**

## Performance

| Metric | Value |
|--------|-------|
| Duration | ~25 min |
| Started | 2026-04-11T01:00:00Z |
| Completed | 2026-04-11T01:25:00Z |
| Tasks | 3 of 3 completed |
| Files modified | 4 (2 created, 2 modified) |
| Tests | 85 → 93 passing (+8 parametrized cases) |

## Acceptance Criteria Results

| Criterion | Status | Notes |
|-----------|--------|-------|
| AC-1: Cache-invariant test covers all 8 mutators | PASS | ctest IDs 86–93 — `AllMutators/CacheInvariantTest.ClearsBothCachesAfterMutation/{loadLocale,mergeLocale,load_json,setLocale_sv,setLocale_vector,setFallbackLocale,configure,reset}` all pass. Both `formatCacheSize()==0` and `translationCacheSize()==0` asserted per-parameter with `<< GetParam().name` failure labels. |
| AC-2: Class-level thread-safety contract documented | PASS | `rg "NOT thread-safe" include/i18ncpp.h` → 1 match at line 133. Docblock immediately above `class I18N {` names all 5 mutable members, lists both valid usage patterns, and enumerates non-const mutators. Mutable-member declarations carry `// NOTE:` cross-reference. 93/93 tests still pass → headers-only change introduced no regressions. |
| AC-3: TSan regression test exists and is build-gated | PASS (config-level) | Default MSVC Debug build produces no `i18ncpp_thread_safety_tests` target (confirmed — `ls build/tests/Debug/` has no `thread_safety` binary). `tests/test_thread_safety.cpp` entire body is under `#ifdef I18N_TSAN_BUILD`, so even if included in a build that doesn't define the macro, it compiles to an empty TU. CMake option `I18N_ENABLE_TSAN` defaults OFF; `if(I18N_ENABLE_TSAN AND NOT MSVC)` block wires `-fsanitize=thread -g` compile/link options and the `I18N_TSAN_BUILD` definition. Runtime TSan execution deferred to Linux/CI (TSK-005 in Phase 4). |

## Accomplishments

- **P-004 closed — cache-invariant parametrized test:** `tests/test_cache_invariant.cpp` (88 LoC) enumerates every public mutator as a `MutatorCase`, warms both caches via `tr("greeting") + formatNumber(1234.56)`, ASSERT_GTs they're non-empty, applies the mutator, EXPECT_EQs both sizes to 0. Each parameter emits a mutator-named failure label. **Reuses only existing fixtures** — `en.json`, `fr.json`, `cs.json`, `merge_extra.json` plus inline JSON — no new fixture files per the plan's fixture-sprawl guard.
- **P-003 Change 1+2 closed — thread-safety docblock:** 22-line `///`-style docblock at `include/i18ncpp.h:131` covering: one-line class description, `\warning` stating NOT thread-safe even for `const` methods, enumeration of all 5 mutable members with roles, consequence statement (bucket-pointer corruption → segfault, not stale data), two valid usage patterns (per-thread / external mutex), and the list of non-const mutators. Member-level `// NOTE:` cross-reference at the mutable block (`i18ncpp.h:203`) points readers back up to the warning.
- **P-003 Change 3 closed at config level — opt-in TSan target:** `tests/test_thread_safety.cpp` (new, 52 LoC) behind `#ifdef I18N_TSAN_BUILD`; runs 8 threads × 10,000 iterations × one `i18n::I18N` per thread with `tr("greeting")` in a hot loop. `tests/CMakeLists.txt` adds `option(I18N_ENABLE_TSAN "..." OFF)` and an `if(I18N_ENABLE_TSAN AND NOT MSVC)` block that builds a **separate** executable with per-target `-fsanitize=thread -g` compile/link flags + `I18N_TSAN_BUILD` definition. The default Windows dev build is binary-identical to pre-plan.
- **93/93 tests green** on MSVC Debug (v143, C++20). 85 Phase 1 baseline + 8 new parametrized instantiations = 93 total. Total test time 0.52s.

## Task Commits

Not yet committed. User preference is `oco --yes` (opencommit) for all commit messages — requires LM Studio local model server running (`lms server status` → `lms server start` if needed). Per user memory: minimize PAUL phase logging during the loop; defer ROADMAP/PROJECT updates to milestone boundary (v0.4.1 end, not Phase 2 end).

| Task | Type | Description |
|------|------|-------------|
| Task 1: Parametrized cache-invariant test | test | Create `tests/test_cache_invariant.cpp` (88 LoC) + new `i18ncpp_cache_invariant_tests` executable in `tests/CMakeLists.txt`. 8 mutator parameters, both caches asserted. |
| Task 2: Thread-safety docblock + member cross-ref | docs | Insert 22-line `///` docblock above `class I18N {` in `include/i18ncpp.h`; add `// NOTE:` above the mutable-member block. |
| Task 3: Opt-in TSan regression test | test+build | Create `tests/test_thread_safety.cpp` (gated behind `#ifdef I18N_TSAN_BUILD`) + `option(I18N_ENABLE_TSAN OFF)` + `if(I18N_ENABLE_TSAN AND NOT MSVC)` block with separate executable, `-fsanitize=thread` flags, and `I18N_TSAN_BUILD` definition. |

## Files Created/Modified

| File | Change | Purpose |
|------|--------|---------|
| `tests/test_cache_invariant.cpp` | Created | Parametrized GTest covering all 8 public mutators — enforces cache-clear invariant at commit time, replacing "by discipline" with "by test enumeration" |
| `tests/test_thread_safety.cpp` | Created | TSan regression guard for the supported "one instance per thread" usage pattern; empty TU unless `I18N_TSAN_BUILD` is defined |
| `include/i18ncpp.h` | Modified | 22-line class-level `\warning` docblock + 3-line `// NOTE:` cross-reference on the mutable-member block |
| `tests/CMakeLists.txt` | Modified | New `i18ncpp_cache_invariant_tests` target + discovery; `option(I18N_ENABLE_TSAN OFF)` block gated `NOT MSVC` with per-target sanitizer flags |

## Decisions Made

| Decision | Rationale | Impact |
|----------|-----------|--------|
| Follow project convention (per-file executables) instead of plan's "same target" wording | Inspecting `tests/CMakeLists.txt` revealed the project uses one `add_executable` per test file (`i18ncpp_tests`, `i18ncpp_plural_tests`, …). Plan's assumption about a shared translation-unit target was wrong. Correct response: match existing style, not wrong plan wording. | Consistency with project structure; adding the cache-invariant test didn't require touching any other test executable's source list |
| Use `"EUR"` instead of `"€"` in the `configure` mutator's inline JSON | MSVC source-encoding hazard — the file is UTF-8 without BOM, and past projects have hit `C4828` / codepage-1251 fallout on non-ASCII string literals. The test's observable behavior is "cache size drops to 0 after configure()", which is indifferent to the symbol's visible glyph. | Zero effect on AC-1; eliminates a platform-specific build risk |
| Preserve the plan's "don't add new fixture files" stance despite P-004 playbook suggesting `de.json` / `en_patch.json` | Existing fixtures (`en.json`, `fr.json`, `cs.json`, `merge_extra.json`) plus inline `i18n::json` object literals cover all 8 mutator cases. Adding new fixture files for single-use-per-test values is fixture sprawl. | Keeps `tests/fixtures/` tightly scoped; Phase 3 P-005 (adversarial fixtures) doesn't have to work around single-use plumbing |
| TSan target is a **separate** executable with per-target flags, not a shared one with global flags | Sanitizer flags must never enter the default MSVC build. Per-target options + explicit `if(I18N_ENABLE_TSAN AND NOT MSVC)` guard means enabling the option on MSVC is a no-op rather than a broken configuration. | Default `cmake --build build` is bit-identical regardless of whether this plan landed |

## Deviations from Plan

### Summary

| Type | Count | Impact |
|------|-------|--------|
| Auto-fixed | 1 | Plan wording about shared test target was wrong; corrected to project convention at apply time |
| Scope additions | 0 | None |
| Deferred | 0 | None |
| Trivial drifts | 1 | `"EUR"` instead of `"€"` in configure mutator — no AC impact |

**Total impact:** Both deviations are apply-time corrections with zero effect on AC-1/AC-2/AC-3. The plan's test infrastructure assumption was wrong; the plan's symbol choice was stylistically risky under MSVC.

### Auto-fixed Issues

**1. [spec] PLAN.md Task 1 assumed shared test executable layout**
- **Found during:** Task 1 apply, after reading `tests/CMakeLists.txt`
- **Issue:** Task 1 action said `"Register the new file in tests/CMakeLists.txt by adding it to the same translation-unit list used by test_format_cache.cpp (same target — do NOT create a second test executable)"`. The actual project layout is one `add_executable` per test file — there is no shared list. The plan's instruction was not implementable.
- **Fix:** Created new `i18ncpp_cache_invariant_tests` target mirroring the existing pattern (source = `test_cache_invariant.cpp`, links `i18ncpp nlohmann_json GTest::gtest_main`, defines `FIXTURES_DIR`, discovered via `gtest_discover_tests`).
- **Files:** `tests/CMakeLists.txt`
- **Verification:** Clean rebuild; new target materialized; 93/93 ctest pass
- **Commit:** part of task commit (not yet committed — see Task Commits section)

### Trivial Drifts (noted for completeness)

**1. Configure mutator symbol — `"EUR"` vs `"€"`**
- **Plan text:** `configure(inline json {{"currency", {{"symbol", "€"}}}})`
- **Actual:** `configure(inline json {{"currency", {{"symbol", "EUR"}}}})`
- **Reason:** MSVC non-ASCII source-encoding robustness; the test's assertion set is indifferent to the symbol value
- **Impact on AC-1:** None — `formatCacheSize() == 0` and `translationCacheSize() == 0` are the assertions, and any valid `configure()` call clears both

### Deferred Items

None — plan executed as written for everything AC-relevant.

## Issues Encountered

| Issue | Resolution |
|-------|------------|
| PLAN.md described a test-target structure that didn't match the actual `tests/CMakeLists.txt` layout | Followed project convention at apply time — created new per-file executable; logged as spec-level plan deviation for UNIFY |

## Next Phase Readiness

**Ready:**
- AEGIS M3/M4 medium findings both closed — P-004 enforced by test enumeration, P-003 by class-level docblock. Phase 3 (Adversarial Tests, P-005) can reuse the existing `tests/fixtures/formats_wrong_type.json` hostile-input precedent from Phase 1 and add type/truncation variants next to it.
- Parametrized test infrastructure in place — Phase 3 can reuse `CacheInvariantTest`-style patterns for adversarial test cases.
- Thread-safety contract is now part of the public header — Phase 4 TSK-004 (CLAUDE.md) can reference it directly via `include/i18ncpp.h:131-152` without restating the contract.
- `I18N_ENABLE_TSAN` CMake option is installed — Phase 4 TSK-005 (CI/CD) can wire a Linux runner that builds with `-DI18N_ENABLE_TSAN=ON` and runs the TSan regression guard.

**Concerns:**
- TSan target is only **config-verified** on Windows (no TSan toolchain available). Runtime verification must happen on Linux/WSL or in CI — gate-passes-but-runtime-untested is explicitly accepted for AC-3 and documented here for UNIFY traceability.
- The `I18N_TSAN_BUILD` macro technically only gates the test body — if a future change moves shared infrastructure outside the `#ifdef`, the file might contaminate the non-TSan `i18ncpp_cache_invariant_tests` TU if anyone accidentally adds it to the shared source list. Mitigation: the plan's boundary explicitly keeps `test_thread_safety.cpp` **only** in the TSan target's source list.
- Empty-TU-when-not-TSan is a cross-check, not a hard guarantee — the CMake gate (`if(I18N_ENABLE_TSAN AND NOT MSVC)`) is the real protection.

**Blockers:** None.

---
*Phase: 02-concurrency-invariants, Plan: 01*
*Completed: 2026-04-11*
