# Benchmark Results — i18ncpp

**Compiler:** MSVC v143 (Visual Studio 2022)
**Platform:** Windows 11 Pro x64 (10.0.22631)
**Build:** Release, C++17
**Iterations:** 100,000 per benchmark (10% warm-up discarded)

---

## v0.2.0 Baseline (Pre-optimization)

*Captured: 2026-04-09, before v0.3 optimization phases*

### Lookup

| Benchmark | ns/op |
|-----------|-------|
| TrSimpleKey | 205 |
| TrNestedKey | 207 |
| TrDeepNested | 209 |
| TrMissingFallback | 315 |
| TrPlural | 1535 |
| KeyExists | 193 |
| KeyExistsMissing | 194 |

### Interpolation

| Benchmark | ns/op |
|-----------|-------|
| InterpolatePositional | 1545 |
| InterpolateTwoParams | 2224 |
| InterpolateNamed | 6775 |
| PluralWithParam | 2266 |
| NoPlaceholders | 214 |

### Formatting

| Benchmark | ns/op |
|-----------|-------|
| FormatNumber | 338 |
| FormatNumberCached | 278 |
| FormatPrice | 284 |
| FormatPriceCached | 277 |
| FormatDate | 131 |
| FormatDateCached | 131 |

---

## v0.3.0 Post-Optimization

*Captured: 2026-04-09, after Phases 2-4 (move semantics, buffer pooling, fold expressions, translation cache)*
*Values are averages of 2 consecutive runs*

### Lookup

| Benchmark | ns/op |
|-----------|-------|
| TrSimpleKey | 22 |
| TrNestedKey | 26 |
| TrDeepNested | 26 |
| TrMissingFallback | 22 |
| TrPlural | 56 |
| KeyExists | 183 |
| KeyExistsMissing | 188 |

### Interpolation

| Benchmark | ns/op |
|-----------|-------|
| InterpolatePositional | 65 |
| InterpolateTwoParams | 125 |
| InterpolateNamed | 6740 |
| PluralWithParam | 99 |
| NoPlaceholders | 26 |

### Formatting

| Benchmark | ns/op |
|-----------|-------|
| FormatNumber | 342 |
| FormatNumberCached | 290 |
| FormatPrice | 278 |
| FormatPriceCached | 275 |
| FormatDate | 139 |
| FormatDateCached | 136 |

---

## v0.3.0 Post-Regex-Removal

*Captured: 2026-04-09, after Phase 6 (replace std::regex with manual parsers)*
*Values are averages of 3 consecutive runs*

### Lookup

| Benchmark | ns/op |
|-----------|-------|
| TrSimpleKey | 20 |
| TrNestedKey | 25 |
| TrDeepNested | 26 |
| TrMissingFallback | 22 |
| TrPlural | 56 |
| KeyExists | 180 |
| KeyExistsMissing | 185 |

### Interpolation

| Benchmark | ns/op |
|-----------|-------|
| InterpolatePositional | 64 |
| InterpolateTwoParams | 123 |
| InterpolateNamed | 489 |
| PluralWithParam | 93 |
| NoPlaceholders | 26 |

---

## Before/After Comparison

### Lookup

| Benchmark | Baseline (ns/op) | Current (ns/op) | Change |
|-----------|-------------------|-----------------|--------|
| TrSimpleKey | 205 | 22 | **-89%** |
| TrNestedKey | 207 | 26 | **-87%** |
| TrDeepNested | 209 | 26 | **-88%** |
| TrMissingFallback | 315 | 22 | **-93%** |
| TrPlural | 1535 | 56 | **-96%** |
| KeyExists | 193 | 183 | -5% |
| KeyExistsMissing | 194 | 188 | -3% |

### Interpolation

| Benchmark | Baseline (ns/op) | Current (ns/op) | Change |
|-----------|-------------------|-----------------|--------|
| InterpolatePositional | 1545 | 64 | **-96%** |
| InterpolateTwoParams | 2224 | 123 | **-94%** |
| InterpolateNamed | 6775 | 489 | **-93%** |
| PluralWithParam | 2266 | 93 | **-96%** |
| NoPlaceholders | 214 | 26 | **-88%** |

### Formatting

| Benchmark | Baseline (ns/op) | Current (ns/op) | Change |
|-----------|-------------------|-----------------|--------|
| FormatNumber | 338 | 342 | +1% |
| FormatNumberCached | 278 | 290 | +4% |
| FormatPrice | 284 | 278 | -2% |
| FormatPriceCached | 277 | 275 | -1% |
| FormatDate | 131 | 139 | +6% |
| FormatDateCached | 131 | 136 | +4% |

---

## Analysis

### Major Wins

- **Translation lookup (tr/trPlural): 87-96% faster.** The translation result cache (Phase 4) delivers massive gains by skipping interpolation entirely on cache hits. TrSimpleKey dropped from 205 to 22 ns/op (~9x faster). TrPlural dropped from 1535 to 56 ns/op (~27x faster).

- **Positional/unnamed interpolation: 94-96% faster.** Buffer pooling (Phase 3) and fold expression optimization (Phase 3) eliminated heap allocations in the hot path. InterpolatePositional dropped from 1545 to 65 ns/op (~24x faster).

- **NoPlaceholders: 88% faster (214 -> 26 ns/op).** Translation cache short-circuits when no interpolation is needed.

### Regex Removal Impact (Phase 6)

- **InterpolateNamed: 6740 → 489 ns/op (93% faster).** Replacing `std::regex` with hand-written character-by-character parsers eliminated the last major bottleneck. The `<regex>` header dependency is fully removed.

### No Change (Expected)

- **KeyExists/KeyExistsMissing:** These call `hasTranslation()` which was already a simple hash lookup. No optimization targeted this path.

- **Formatting:** FormatNumber/Price/Date are unchanged within noise. These were already fast and the formatting cache was implemented in v0.2. No further optimization was applied in v0.3.

### Optimization Techniques and Their Impact

| Technique | Phase | Primary Impact |
|-----------|-------|---------------|
| Move semantics (flattenJson) | 2 | Data loading (not benchmarked in hot path) |
| Composite key buffer reuse | 2 | TrSimpleKey/Nested/Deep: ~87-89% faster |
| Mutable buffer pooling | 3 | InterpolatePositional/TwoParams: ~94-96% faster |
| Fold expression argsToStrings | 3 | PluralWithParam: ~96% faster |
| Translation result cache | 4 | TrSimpleKey/Plural/Fallback: ~89-96% faster |
| Manual parsers (regex removal) | 6 | InterpolateNamed: ~93% faster |
