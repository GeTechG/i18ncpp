# Project State

## Project Reference

See: .paul/PROJECT.md (updated 2026-04-09)

**Core value:** C++ developers can localize their applications with translation, pluralization, and locale-aware formatting — without heavy dependencies like ICU.
**Current focus:** v0.4 Phase 4 — API Modernization

## Current Position

Milestone: v0.4 C++20 Modernization
Phase: 4 of 4 (API Modernization)
Plan: Not started
Status: Ready to plan
Last activity: 2026-04-09 — Phase 3 complete, transitioned to Phase 4

Progress:
- v0.4 C++20 Modernization: [███████░░░] 75%
- Phase 4: [░░░░░░░░░░] 0%

## Loop Position

Current loop state:
```
PLAN ──▶ APPLY ──▶ UNIFY
  ○        ○        ○     [Ready for Phase 4 planning]
```

## Accumulated Context

### Decisions
- C++20 standard (migrated from C++17), CMake + Conan build, nlohmann/json as sole dependency
- NumberConfig requires explicit 5-arg constructor under C++20 (aggregate init no longer works with user-declared special members)
- API backward compatibility required (tr, trPlural signatures unchanged)
- Google Test via FetchContent (not Conan) for test infrastructure
- enable_testing() in root CMakeLists.txt for MSVC multi-config ctest discovery
- Flat storage: `unordered_map<string, unordered_map<string, string>>` — outer=locale, inner=flattened key, value=pure string (no JSON in runtime)
- Plural/variant objects fully flattened: `key.one`, `key.other`, etc. — plural form selection happens at lookup time by building composite key
- Leaf object heuristic: objects where all values are strings are plural/variant forms; objects with nested objects are containers
- Formatting cache: mutable unordered_map for formatNumber/Price/Date results; invalidated on all locale/config mutation paths
- Cache skips formatDate(nullptr) — current time is non-deterministic
- Custom chrono-based benchmark harness (zero new dependencies)
- Transparent hash unblocked: C++20 migration complete, heterogeneous find() now available
- Dual flattenJson overloads: const for borrowed JSON, mutable for locally-parsed JSON (move semantics)
- Composite key buffer pattern: assign+append reusable buffer instead of operator+ concatenation
- Mutable member buffer pooling for interpolation (interpolateBuf_, interpolateBuf2_, extendedParamsBuf_)
- Fold expression for argsToStrings: single-pass O(n) instead of recursive O(n^2)
- Translation result cache: mutable unordered_map for tr/trPlural results, null-byte-separated cache keys
- Manual parsers replace all 4 std::regex patterns — `<regex>` dependency fully removed
- Skip second interpolation pass (%<key>.fmt) when text has no %< pattern
- translate() avoids vector copy in common case (no "locale" param override)
- std::format for formatCache_ key construction (replaces manual string concatenation)
- Constexpr sorted array for plural rule locale lookup (replaces runtime unordered_map)
- %B specifier added to formatDateWithConfig (maps to full month name, same as %F)

### Deferred Issues
None.

### Git State
Last commit: 5cca94c
Branch: master
Feature branches merged: none

### Blockers/Concerns
None.

## Session Continuity

Last session: 2026-04-09
Stopped at: Phase 3 complete, ready to plan Phase 4
Next action: /paul:plan for Phase 4
Resume file: .paul/ROADMAP.md

---
*STATE.md — Updated after every significant action*
