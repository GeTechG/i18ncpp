---
phase: 01-cpp20-migration
plan: 01
subsystem: infra
tags: [cmake, c++20, build-config, aggregate-init]

requires:
  - phase: none
    provides: n/a
provides:
  - C++20 build configuration
  - Foundation for heterogeneous lookup, std::format, std::span
affects: [02-transparent-hash, 03-string-view, 04-format-modernization]

tech-stack:
  added: []
  patterns: [explicit constructors for config structs to avoid C++20 aggregate issues]

key-files:
  created: []
  modified: [CMakeLists.txt, include/i18ncpp.h]

key-decisions:
  - "NumberConfig needs explicit 5-arg constructor for C++20 aggregate init compat"
  - "conanfile.py left unchanged — CMakeLists.txt is authority on C++ standard"

patterns-established:
  - "Config structs with user-declared special members need explicit parameterized constructors under C++20"

duration: 5min
completed: 2026-04-09T13:30:00Z
---

# Phase 1 Plan 01: C++17 to C++20 Migration Summary

**Switched build from C++17 to C++20 with aggregate initialization fix for NumberConfig**

## Performance

| Metric | Value |
|--------|-------|
| Duration | ~5min |
| Completed | 2026-04-09 |
| Tasks | 2 completed |
| Files modified | 2 |

## Acceptance Criteria Results

| Criterion | Status | Notes |
|-----------|--------|-------|
| AC-1: C++20 Standard Set in Build | Pass | CMAKE_CXX_STANDARD 20 at line 4 |
| AC-2: All Existing Tests Pass | Pass | 82/82 tests pass, 0 failures |
| AC-3: Benchmarks Compile | Pass | bench_lookup, bench_interpolation, bench_formatting all built |

## Accomplishments

- C++20 standard enabled across the entire build
- All 82 existing tests pass without regression
- All 3 benchmark targets compile successfully
- Identified and fixed C++20 aggregate initialization breaking change in NumberConfig

## Files Created/Modified

| File | Change | Purpose |
|------|--------|---------|
| `CMakeLists.txt` | Modified | CMAKE_CXX_STANDARD 17 → 20 |
| `include/i18ncpp.h` | Modified | Added 5-arg constructor to NumberConfig for C++20 compat |

## Decisions Made

| Decision | Rationale | Impact |
|----------|-----------|--------|
| Add explicit NumberConfig constructor | C++20 no longer allows aggregate init for types with user-declared constructors | Other config structs (CurrencyConfig, DateTimeConfig) not affected — no aggregate init usage |
| Keep conanfile.py unchanged | CMakeLists.txt overrides Conan's default C++ standard; changing conanfile would require profile/toolchain regen | Conan profile still says cppstd=14 but this is overridden at build time |

## Deviations from Plan

### Summary

| Type | Count | Impact |
|------|-------|--------|
| Auto-fixed | 1 | Essential C++20 compat fix |
| Scope additions | 0 | — |
| Deferred | 0 | — |

**Total impact:** Essential fix, no scope creep

### Auto-fixed Issues

**1. [C++20 Compat] NumberConfig aggregate initialization failure**
- **Found during:** Task 2 (Build and run tests)
- **Issue:** MSVC C2440 error — C++20 changed aggregate rules; NumberConfig with user-declared copy/move constructors is no longer an aggregate
- **Fix:** Added explicit 5-arg constructor: `NumberConfig(string, string, int, string, string)`
- **Files:** `include/i18ncpp.h`
- **Verification:** Full rebuild + 82/82 tests pass

### Plan Inaccuracy

- `conanfile.py` listed in plan frontmatter `files_modified` but no changes were needed or made

## Issues Encountered

| Issue | Resolution |
|-------|------------|
| C++20 aggregate init rules broke NumberConfig{...} construction in formatPriceWithConfig | Added parameterized constructor to NumberConfig |

## Next Phase Readiness

**Ready:**
- C++20 features now available: heterogeneous lookup, std::format, std::span, constexpr improvements
- All tests green, clean baseline for next phase

**Concerns:**
- CurrencyConfig and DateTimeConfig have same user-declared constructor pattern — if future code adds aggregate init for them, same fix needed
- Conan profile defaults to cppstd=14; may want to align profile in future

**Blockers:**
- None

---
*Phase: 01-cpp20-migration, Plan: 01*
*Completed: 2026-04-09*
