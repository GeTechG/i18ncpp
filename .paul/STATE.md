# Project State

## Project Reference

See: .paul/PROJECT.md (updated 2026-04-09)

**Core value:** C++ developers can localize their applications with translation, pluralization, and locale-aware formatting — without heavy dependencies like ICU.
**Current focus:** v0.4 complete — no active milestone

## Current Position

Milestone: Awaiting next milestone
Phase: None active
Plan: None
Status: Milestone v0.4 C++20 Modernization complete — ready for next
Last activity: 2026-04-09 — Milestone completed

Progress:
- v0.4 C++20 Modernization: [██████████] 100% ✓

## Loop Position

Current loop state:
```
PLAN ──▶ APPLY ──▶ UNIFY
  ○        ○        ○     [Milestone complete - ready for next]
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

### Deferred Issues
None.

### Git State
Last commit: 3cca973
Branch: master
Feature branches merged: none

### Blockers/Concerns
None.

## Session Continuity

Last session: 2026-04-09
Stopped at: Milestone v0.4 C++20 Modernization complete
Next action: /paul:discuss-milestone or /paul:milestone
Resume file: .paul/MILESTONES.md

---
*STATE.md — Updated after every significant action*
