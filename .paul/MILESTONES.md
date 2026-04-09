# Milestones

Completed milestone log for this project.

| Milestone | Completed | Duration | Stats |
|-----------|-----------|----------|-------|
| v0.3 Deep Performance | 2026-04-09 | 1 day | 6 phases, 6 plans |
| v0.2 Performance & Reliability | Pre-tracking | — | 3 phases, 5 plans |
| v0.1 Initial Release | Pre-tracking | — | — |

---

## v0.3 Deep Performance

**Completed:** 2026-04-09
**Duration:** 1 day (all 6 phases)

### Stats

| Metric | Value |
|--------|-------|
| Phases | 6 |
| Plans | 6 |
| Files changed | 9 |

### Key Accomplishments

- Custom benchmark harness with 18 micro-benchmarks establishing baseline performance data
- Move semantics for JSON string extraction and reusable composite key buffers in lookup hot paths
- Mutable member buffer pooling for interpolation; O(n) fold expression replacing O(n^2) recursive argsToStrings
- Translation result cache (tr/trPlural) with automatic invalidation on all 7 mutation paths
- Manual parsers replacing all 4 std::regex patterns — `<regex>` dependency fully removed
- InterpolateNamed: 6740 -> 489 ns/op (93% faster)
- Lookup: 87-96% faster across all benchmarks (TrSimpleKey 205 -> 22 ns/op)
- Positional interpolation: 94-96% faster (1545 -> 65 ns/op)

### Key Decisions

| Decision | Rationale |
|----------|-----------|
| Custom chrono harness over Google Benchmark | Zero new dependencies |
| Transparent hash deferred to C++20 | C++17 unordered_map lacks heterogeneous find() |
| Dual flattenJson overloads (const + mutable) | Borrowed vs owned data separation |
| Null-byte cache key separators | Unambiguous encoding, cannot appear in user strings |
| Manual parsers over std::regex | 93% improvement; removes entire `<regex>` dependency |

---

## v0.2 Performance & Reliability

**Completed:** Pre-tracking
**Phases:** Tests, Flat Storage, Formatting Cache

---

## v0.1 Initial Release

**Completed:** Pre-tracking
**Status:** Shipped

---
*Created: 2026-04-09*
