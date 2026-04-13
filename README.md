# i18ncpp

## Overview

i18ncpp provides a powerful and flexible solution for adding internationalization (i18n) capabilities to your C++ projects. It offers a wide range of features for translation, pluralization, number formatting, currency formatting, and date/time formatting.

This library is a C++ implementation of the [smiti18n](https://github.com/Oval-Tutu/smiti18n/tree/master) Lua library, providing similar functionality with a C++ interface.

## Features

- **Translation Management**: Load and manage translations from JSON files
- **Variable Interpolation**: Insert dynamic values into translations
- **Pluralization**: Handle grammatically correct plural forms for different languages
- **Locale Fallbacks**: Automatically use fallback translations when a specific locale is not available
- **Nested Keys**: Support for structured translation hierarchies
- **Variants**: Choose translations based on contextual parameters
- **Number Formatting**: Format numbers with locale-specific separators
- **Currency Formatting**: Format monetary values with appropriate currency symbols
- **Date/Time Formatting**: Format dates and times according to locale preferences

## Installation

### Using CMake

```bash
# Clone the repository
git clone https://github.com/yourusername/i18ncpp.git

# Create a build directory
mkdir build && cd build

# Configure and build
cmake ..
cmake --build .

# Install (optional)
cmake --install .
```

### Dependencies

- C++20 or higher
- [nlohmann/json](https://github.com/nlohmann/json) for JSON parsing

### Toolchain

Uses `std::format` on toolchains that provide it (GCC 13+, Clang 17+, MSVC 19.29+ / Visual Studio 2022 17.0+). On older toolchains the build auto-fetches [{fmt}](https://github.com/fmtlib/fmt) via CMake `FetchContent` — no Conan change required, and `{fmt}` is never linked when `std::format` is available.

## Basic Usage

```cpp
#include <i18ncpp.h>
#include <iostream>

int main() {
    i18n::I18N i18n;

    // Load translations from a JSON file
    i18n.loadLocaleFromFile("en.json");
    i18n.loadLocaleFromFile("fr.json");

    // Set active locale
    i18n.setLocale("en");

    // Basic translation
    std::cout << i18n.tr("greeting") << std::endl; // Hello!

    // Translation with parameters (variadic convenience)
    std::cout << i18n.trv("welcome", "John") << std::endl; // Welcome, John!

    // Or with an initializer_list
    std::cout << i18n.tr("welcome", {"John"}) << std::endl; // Welcome, John!

    // Pluralization
    std::cout << i18n.trPlural("items", 1) << std::endl; // 1 item
    std::cout << i18n.trPlural("items", 5) << std::endl; // 5 items

    // Change locale
    i18n.setLocale("fr");
    std::cout << i18n.tr("greeting") << std::endl; // Bonjour!

    return 0;
}
```

## Translation Files Format

Translation files use JSON format. Here's an example:

```json
{
  "greeting": "Hello!",
  "welcome": "Welcome, {}!",
  "items": {
    "one": "{} item",
    "other": "{} items"
  },
  "user": {
    "login": "Log in",
    "logout": "Log out",
    "profile": "User profile"
  },
  "_formats": {
    "number": {
      "decimal_symbol": ".",
      "thousand_separator": ","
    },
    "currency": {
      "symbol": "$",
      "name": "US Dollar",
      "short_name": "USD"
    }
  }
}
```

## API Reference

### Core Methods

- `loadLocale(locale, filePath)`: Load translations for a specific locale from a file
- `loadLocaleFromFile(filePath)`: Load translations, extracting locale from filename
- `setLocale(locale)`: Set the active locale
- `setFallbackLocale(locale)`: Set the fallback locale
- `getLocale()`: Get the current active locale
- `getFallbackLocale()`: Get the current fallback locale

### Translation Methods

All translation methods are `const` and accept parameters as a `std::span` or
`std::initializer_list` of strings. Per-argument overloads were removed in v0.4
in favor of the span/initializer_list/variadic form below.

- `tr(key)`: Get a translation by key (no parameters)
- `tr(key, std::span<const std::string>)`: Translation with parameters from any contiguous container
- `tr(key, std::initializer_list<std::string>)`: Translation with an inline parameter list, e.g. `tr("welcome", {"John"})`
- `trv(key, args...)`: Variadic convenience — stringifies each argument and forwards to `tr`
- `trPlural(key, count)`: Pluralized translation with no parameters
- `trPlural(key, count, std::span<const std::string>)`: Pluralized translation with parameters
- `trPlural(key, count, std::initializer_list<std::string>)`: Pluralized translation with an inline parameter list
- `trPluralv(key, count, args...)`: Variadic convenience for pluralized translations
- `translate(key, json params)`: Legacy JSON-based interpolation (kept for named/formatted parameters)

### Formatting Methods

- `formatNumber(number)`: Format a number according to locale settings
- `formatPrice(amount)`: Format a monetary amount with currency symbol
- `formatDate(pattern, date)`: Format a date/time value
- `configure(formats)`: Configure formatting options

## Advanced Usage

### Pluralization Rules

The library supports pluralization rules for different languages, including:

- English-like languages (en, de, es, etc.)
- Slavic languages (ru, uk, pl, etc.)
- Arabic
- French
- Czech/Slovak
- And more

### Interpolation Formats

Several interpolation formats are supported:

- `{}` or `{0}`, `{1}`, etc. for indexed parameters
- `%{key}` for named parameters
- `%<key>.fmt` for formatted parameters where `fmt` is a format specifier:
  - `%<num>.d` - integer format
  - `%<num>.f` - floating-point format
  - `%<num>.s` - string format

## License

[MIT License](LICENSE)

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.