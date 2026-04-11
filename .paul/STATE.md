# Project State

## Project Reference

See: .paul/PROJECT.md (updated 2026-04-09)

**Core value:** C++ developers can localize their applications with translation, pluralization, and locale-aware formatting — without heavy dependencies like ICU.
**Current focus:** v0.4.1 Hardening & Infrastructure — close AEGIS medium findings + install CI/CD & docs

## Current Position

Milestone: v0.4.1 Hardening & Infrastructure — **complete**
Phase: — (no active phase)
Plan: — (no active plan)
Status: IDLE — milestone closed, ready for next /paul:milestone or /paul:plan for a new cycle
Last activity: 2026-04-11 — v0.4.1 transition complete. ROADMAP archived, PROJECT evolved, dirty tree reconciled into 4 phase-aligned commits.

Progress:
- v0.4.1 Hardening & Infrastructure: [██████████] 100% COMPLETE

## Loop Position

Current loop state:
```
PLAN ──▶ APPLY ──▶ UNIFY
  ○        ○        ○     [IDLE — v0.4.1 milestone closed]
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

### Deferred Issues
- load(const json&) partial-merge localesData rollback → v0.5 (cache invariant is preserved; merged locales from earlier iterations remain)
- Windows Unicode-path regression test with 日本語.json fixture → optional per P-002, deferred to v0.5
- AEGIS P-003 (thread-safety contract) and P-004 (cache-invariant parametrized test) → Phase 2

### Git State
Last commit: 33e48c9 (Phase 1 Boundary Correctness)
Branch: master
Feature branches merged: none

### Blockers/Concerns
None.

## Session Continuity

Last session: 2026-04-11
Stopped at: v0.4.1 milestone fully closed. Four phase-aligned commits landed (ef618f5 ROADMAP install → c6b46be tests → 2b02271 docs+CI → state sync commit pending). ROADMAP archived, PROJECT.md evolved, paul.json marked complete.
Next action: /paul:milestone to pick next milestone (v0.4.2? v0.5?), or /paul:plan directly if scope is already known. Remaining dirty items outside milestone scope: .gitignore (.aegis/ entry), sonar-project.properties, .scannerwork/ — follow-up chore, not urgent.
Resume file: (none — IDLE)

---
*STATE.md — Updated after every significant action*
