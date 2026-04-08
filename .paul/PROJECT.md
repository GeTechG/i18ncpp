# i18ncpp

## What This Is

A lightweight C++ internationalization (i18n) library — a port of smiti18n (Lua) to C++. Provides translation management from JSON files, variable interpolation, pluralization with rules for many languages, locale fallbacks, nested keys, contextual variants, and locale-aware formatting for numbers, currencies, and dates/times.

## Core Value

C++ developers can localize their applications with translation, pluralization, and locale-aware formatting — without heavy dependencies like ICU.

## Current State

| Attribute | Value |
|-----------|-------|
| Type | Application (C++ library) |
| Version | 0.2.0-dev |
| Status | In Progress |
| Last Updated | 2026-04-09 |

## Requirements

### Core Features

- Load and manage translations from JSON files with multiple locales, fallback locale, and merge support
- Variable interpolation (positional, named, formatted) and pluralization with language-specific rules
- Locale-aware formatting for numbers, currencies, and date/time
- Nested translation keys (dot notation) and contextual variants

### Validated (Shipped)
- Test coverage for translation basics (tr, nested keys, fallback, locale switching) — Phase 1
- Test coverage for pluralization (en, ru, fr, pl, cs, ar rules) — Phase 1
- Test coverage for interpolation (positional, unnamed, named, formatted) — Phase 1
- Test coverage for formatting (number, currency, date/time with named and custom patterns) — Phase 1
- Test coverage for locale management (merge, load-from-file, reset) — Phase 1

### Active (In Progress)
- Sub-microsecond tr() performance (currently ~20-170us)

### Planned (Next)
- Flat storage refactoring (Phase 2)
- Formatting cache (Phase 3)

### Out of Scope
- ICU dependency
- Contextual variant testing (lower priority, deferred)

## Constraints

### Technical Constraints
- Only nlohmann/json as external dependency

### Business Constraints
- None identified

## Key Decisions

| Decision | Rationale | Date | Status |
|----------|-----------|------|--------|
| C++17 standard | Modern features without cutting-edge compiler requirements | 2026-04-09 | Active |
| nlohmann/json for JSON | Widely used, header-only, well-tested | 2026-04-09 | Active |
| CMake + Conan | Standard C++ build and package management | 2026-04-09 | Active |

## Success Metrics

| Metric | Target | Current | Status |
|--------|--------|---------|--------|
| Correctness | All translations resolve correctly | 75 tests passing | In progress |
| Performance | Fast locale resolution and formatting | ~20-170us per tr() | Not started |

## Tech Stack / Tools

| Layer | Technology | Notes |
|-------|------------|-------|
| Language | C++17 | Modern standard |
| Build | CMake | Cross-platform build |
| Package Manager | Conan | Dependency management |
| JSON | nlohmann/json | Only external dependency |
| Testing | Google Test v1.14.0 | Via FetchContent |

---
*Last updated: 2026-04-09 after Phase 1*
