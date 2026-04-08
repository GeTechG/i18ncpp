# Roadmap: i18ncpp

## Overview

Build a lightweight, dependency-minimal C++ i18n library that provides translation, pluralization, and locale-aware formatting — evolving from initial implementation toward a tested, documented, production-ready library.

## Current Milestone

**v0.2 Performance & Reliability** (v0.2.0)
Status: 🚧 In Progress
Phases: 1 of 3 complete

## Phases

| Phase | Name | Plans | Status | Completed |
|-------|------|-------|--------|-----------|
| 1 | Tests | 3 (01-translation, 02-plural+interp, 03-formatting) | ✅ Complete | 2026-04-09 |
| 2 | Flat Storage | TBD | Not started | - |
| 3 | Formatting Cache | TBD | Not started | - |

## Phase Details

### Phase 1: Tests

Focus: Test suite covering current behavior (tr, trPlural, number/currency/date formatting, merge, fallback) before refactoring.
Plans: TBD (defined during /paul:plan)

### Phase 2: Flat Storage

Focus: Replace nested JSON traversal with flat `unordered_map<string, string>` for O(1) lookup. Flatten at load/merge time.
Plans: TBD (defined during /paul:plan)

### Phase 3: Formatting Cache

Focus: Cache interpolated/formatted strings to avoid repeated work per frame. Invalidate on locale change.
Plans: TBD (defined during /paul:plan)

## Completed Milestones

### v0.1 Initial Release (v0.1.0)
Status: ✅ Complete (shipped)

---
*Roadmap updated: 2026-04-09*
