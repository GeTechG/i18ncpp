# Project State

## Project Reference

See: .paul/PROJECT.md (updated 2026-04-09)

**Core value:** C++ developers can localize their applications with translation, pluralization, and locale-aware formatting — without heavy dependencies like ICU.
**Current focus:** v0.4.1 Hardening & Infrastructure — close AEGIS medium findings + install CI/CD & docs

## Current Position

Milestone: v0.4.1 Hardening & Infrastructure
Phase: 2 of 4 (Concurrency & Invariants) — Not started
Plan: none yet
Status: Ready to plan Phase 2
Last activity: 2026-04-11 — Phase 1 (Boundary Correctness) complete; transitioned to Phase 2

Progress:
- v0.4.1 Hardening & Infrastructure: [███░░░░░░░] 25%
- Phase 1: [██████████] 100% — complete
- Phase 2: [░░░░░░░░░░] 0%

## Loop Position

Current loop state:
```
PLAN ──▶ APPLY ──▶ UNIFY
  ○        ○        ○     [IDLE - ready to plan Phase 2]
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

### Deferred Issues
- load(const json&) partial-merge localesData rollback → v0.5 (cache invariant is preserved; merged locales from earlier iterations remain)
- Windows Unicode-path regression test with 日本語.json fixture → optional per P-002, deferred to v0.5
- AEGIS P-003 (thread-safety contract) and P-004 (cache-invariant parametrized test) → Phase 2

### Git State
Last commit: 3cca973
Branch: master
Feature branches merged: none

### Blockers/Concerns
None.

## Session Continuity

Last session: 2026-04-11
Stopped at: Phase 1 (Boundary Correctness) complete, transitioned to Phase 2. PROJECT.md/ROADMAP.md intentionally deferred to milestone boundary (per user feedback_paul_state_redundancy).
Next action: /paul:plan for Phase 2 (Concurrency & Invariants — P-004 cache-invariant parametrized test → P-003 thread-safety contract + TSan test)
Resume file: .paul/ROADMAP.md (Phase 2 section)

---
*STATE.md — Updated after every significant action*
