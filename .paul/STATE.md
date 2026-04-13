# Project State

## Project Reference

See: .paul/PROJECT.md (updated 2026-04-09)

**Core value:** C++ developers can localize their applications with translation, pluralization, and locale-aware formatting — without heavy dependencies like ICU.
**Current focus:** v0.4.2 complete. Awaiting next milestone.

## Current Position

Milestone: Awaiting next milestone
Phase: None active
Plan: None
Status: Milestone v0.4.2 complete — ready for next
Last activity: 2026-04-13 — v0.4.2 closed, archived, and tagged

Progress:
- v0.4.2 Correctness & Portability: [██████████] 100% ✓

## Loop Position

Current loop state:
```
PLAN ──▶ APPLY ──▶ UNIFY
  ○        ○        ○     [Milestone complete — ready for next]
```

## Accumulated Context

### Decisions
- C++20 standard (migrated from C++17), CMake + Conan build, nlohmann/json as sole dependency
- NumberConfig requires explicit 5-arg constructor under C++20 (aggregate init no longer works with user-declared special members)
- Google Test via FetchContent (not Conan) for test infrastructure
- enable_testing() in root CMakeLists.txt for MSVC multi-config ctest discovery
- Flat storage: `unordered_map<string, unordered_map<string, string>>` — outer=locale, inner=flattened key, value=pure string (no JSON in runtime)
- Transparent hash: StringHash/StringEqual with is_transparent enable zero-allocation find() on all maps
- std::format for formatCache_ key construction
- Constexpr sorted array for plural rule locale lookup
- std::span for tr/trPlural params: zero-copy parameter passing from any contiguous container
- Simplified overload set: removed individual-param overloads; initializer_list + trv cover all cases
- **[Phase 1]** configure() copy-and-swap: stages into local `FormatConfig tmp`, catches `json::exception`, commits via move + cache-clear as nothrow window (AEGIS M1 closed)
- **[Phase 1]** JSON boundary catches widened from `json::parse_error` to `json::exception` in loadLocale/mergeLocale/load(json) — only `I18NError` crosses the public API (AEGIS M2 closed)
- **[Phase 1]** `std::ifstream{std::filesystem::path(filePath)}` (brace-init to avoid MSVC most-vexing parse) — handles non-null-terminated string_view and Windows Unicode paths
- **[Phase 1]** `load(const json&)` clears caches on throw but does not roll back partial localesData merges — deferred to v0.5 per P-001 Change 4
- **[Phase 2]** Cache-clear invariant enforced by parametrized test enumeration of all 8 public mutators (AEGIS P-004 closed, D-004: tests over generation counter)
- **[Phase 2]** Thread-safety contract documented at class boundary via `\warning` docblock; resolution (1) "document the precondition" chosen over (2) thread_local or (3) internal sync (AEGIS P-003 closed)
- **[Phase 2]** TSan regression test is a separate CMake-option-gated executable (`I18N_ENABLE_TSAN=OFF`, `NOT MSVC`) — sanitizer flags never contaminate the default build
- **[Phase 2]** Project convention: one `add_executable` per test file — new test files get their own target, never share translation units
- **[Phase 3]** AC-3 post-throw invariant is "unchanged" (not "cleared"): Phase 1's configure() copy-and-swap rollback preserves cache state on throw. Plan 03-01's AC-3 was patched during APPLY to snapshot pre-throw sizes and assert equality. Any future plan touching rollback assertions must classify "unchanged vs cleared" explicitly
- **[Phase 3]** F-APP-002 resolved at test level: flattenJson silently skips non-string leaves (src/i18ncpp.cpp:202-228) — pinned by HostileInput.LeafIsIntSilentlySkipped
- **[v0.4.2 Phase 1]** load(json) replace semantics: localesData[locale].clear() before flattenJson — no stale keys survive reload (F1 closed)
- **[v0.4.2 Phase 1]** baselineConfig_ member stores pristine FormatConfig; setLocale resets defaultConfig to it when locale has no per-locale config; reset() clears both (F2 closed)
- **[v0.4.2 Phase 1]** toString template: bool branch before is_arithmetic_v check (bool is arithmetic in C++) — F3 closed
- **[v0.4.2 Phase 1]** keyExists checks `key` AND `key.other` in each fallback locale — matches tr() .other fallback (F4 closed); full trPlural plural-form resolution intentionally NOT replicated
- **[v0.4.2 Phase 2]** std::format policy: fmt-fallback chosen over hard floor. `i18n_fmt::format` wraps std::format or fmt::format via `__cpp_lib_format` feature probe; fmt fetched via CMake FetchContent only when needed; Conan unchanged. PROJECT.md constraint amended to allow fmt as conditional fallback

### Deferred Issues
- load(const json&) partial-merge localesData rollback → v0.5 (cache invariant is preserved; merged locales from earlier iterations remain)
- Windows Unicode-path regression test with 日本語.json fixture → optional per P-002, deferred to v0.5
- AEGIS P-003 (thread-safety contract) and P-004 (cache-invariant parametrized test) → Phase 2

### Git State
Last commit: 530f4be (v0.4.2 Phase 1 Correctness — F1–F4 + G1–G4)
Branch: master
Feature branches merged: none

### Blockers/Concerns
None.

## Session Continuity

Last session: 2026-04-13
Stopped at: Milestone v0.4.2 complete, tagged, archived
Next action: /paul:discuss-milestone or /paul:milestone
Resume file: .paul/MILESTONES.md

---
*STATE.md — Updated after every significant action*
