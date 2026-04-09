# Project State

## Project Reference

See: .paul/PROJECT.md (updated 2026-04-09)

**Core value:** C++ developers can localize their applications with translation, pluralization, and locale-aware formatting — without heavy dependencies like ICU.
**Current focus:** v0.3 Deep Performance — Phase 2: Zero-copy & Move Semantics

## Current Position

Milestone: v0.3 Deep Performance
Phase: 2 of 5 (Zero-copy & Move Semantics)
Plan: Not started
Status: Ready to plan
Last activity: 2026-04-09 — Phase 1 complete, transitioned to Phase 2

Progress:
- v0.3 Deep Performance: [██░░░░░░░░] 20%
- Phase 1: [██████████] 100% ✅
- Phase 2: [░░░░░░░░░░] 0%

## Loop Position

Current loop state:
```
PLAN ──▶ APPLY ──▶ UNIFY
  ○        ○        ○     [Ready for Phase 2 PLAN]
```

## Accumulated Context

### Decisions
- C++17 standard, CMake + Conan build, nlohmann/json as sole dependency
- API backward compatibility required (tr, trPlural signatures unchanged)
- Google Test via FetchContent (not Conan) for test infrastructure
- enable_testing() in root CMakeLists.txt for MSVC multi-config ctest discovery
- Flat storage: `unordered_map<string, unordered_map<string, string>>` — outer=locale, inner=flattened key, value=pure string (no JSON in runtime)
- Plural/variant objects fully flattened: `key.one`, `key.other`, etc. — plural form selection happens at lookup time by building composite key
- Leaf object heuristic: objects where all values are strings are plural/variant forms; objects with nested objects are containers
- Formatting cache: mutable unordered_map for formatNumber/Price/Date results; invalidated on all locale/config mutation paths
- Cache skips formatDate(nullptr) — current time is non-deterministic
- Custom chrono-based benchmark harness (zero new dependencies)
- Regex interpolation identified as #1 bottleneck (~6775 ns/op named, ~1545 ns/op positional)

### Deferred Issues
- Default DateTimeConfig.long_date uses %B specifier which is unhandled in formatDateWithConfig — only %F maps to full month name.

### Git State
Last commit: e52c4bf
Branch: master
Feature branches merged: none

### Blockers/Concerns
None.

## Session Continuity

Last session: 2026-04-09
Stopped at: Phase 1 complete, ready to plan Phase 2
Next action: /paul:plan for Phase 2
Resume file: .paul/ROADMAP.md

---
*STATE.md — Updated after every significant action*
