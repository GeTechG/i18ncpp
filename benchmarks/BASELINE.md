# Benchmark Baseline — v0.2.0

**Date:** 2026-04-09
**Compiler:** MSVC v143 (Visual Studio 2022)
**Platform:** Windows 11 Pro x64 (10.0.22631)
**Build:** Release, C++17
**Iterations:** 100,000 per benchmark

## Lookup

| Benchmark | ns/op |
|-----------|-------|
| TrSimpleKey | 205 |
| TrNestedKey | 207 |
| TrDeepNested | 209 |
| TrMissingFallback | 315 |
| TrPlural | 1535 |
| KeyExists | 193 |
| KeyExistsMissing | 194 |

## Interpolation

| Benchmark | ns/op |
|-----------|-------|
| InterpolatePositional | 1545 |
| InterpolateTwoParams | 2224 |
| InterpolateNamed | 6775 |
| PluralWithParam | 2266 |
| NoPlaceholders | 214 |

## Formatting

| Benchmark | ns/op |
|-----------|-------|
| FormatNumber | 338 |
| FormatNumberCached | 278 |
| FormatPrice | 284 |
| FormatPriceCached | 277 |
| FormatDate | 131 |
| FormatDateCached | 131 |

## Notes

- **Flat lookup is fast:** ~200ns for simple/nested keys, O(1) hash map working well
- **Fallback adds ~50%:** 315ns vs 205ns due to checking multiple locales
- **TrPlural is expensive:** 1535ns — regex-based interpolation dominates
- **Named interpolation is the bottleneck:** 6775ns — regex matching for `%{key}` patterns
- **Formatting cache shows minimal benefit:** ~338 vs 278ns for numbers — cache overhead vs computation is close
- **FormatDate is cheap:** 131ns — strftime is efficient, cache adds no measurable benefit
- **Interpolation regex is the #1 optimization target** for Phases 2-4
