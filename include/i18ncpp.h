#pragma once

#ifndef I18NCPP_H
#define I18NCPP_H

#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <ctime>
#include <optional>
#include <memory>
#include <initializer_list>
#include <span>
#include <nlohmann/json.hpp>
#include <sstream>
#include <tuple>
#include <type_traits>
#include <algorithm>
#include <cctype>

namespace i18n {

using json = nlohmann::json;

struct StringHash {
    using is_transparent = void;
    size_t operator()(std::string_view sv) const noexcept {
        return std::hash<std::string_view>{}(sv);
    }
    size_t operator()(const std::string& s) const noexcept {
        return std::hash<std::string_view>{}(std::string_view(s));
    }
};

struct StringEqual {
    using is_transparent = void;
    bool operator()(std::string_view a, std::string_view b) const noexcept {
        return a == b;
    }
};

class I18NError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

struct CurrencyConfig {
    std::string symbol = "XXX";
    std::string name = "Currency";
    std::string short_name = "XXX";
    std::string decimal_symbol = ".";
    std::string thousand_separator = " ";
    int fract_digits = 2;
    std::string positive_symbol = "";
    std::string negative_symbol = "-";
    std::string positive_format = "%c %p%q";
    std::string negative_format = "%c %p%q";

    CurrencyConfig() = default;
    CurrencyConfig(CurrencyConfig&&) noexcept = default;
    CurrencyConfig& operator=(CurrencyConfig&&) noexcept = default;
    CurrencyConfig(const CurrencyConfig&) = default;
    CurrencyConfig& operator=(const CurrencyConfig&) = default;
};

struct NumberConfig {
    std::string decimal_symbol = ".";
    std::string thousand_separator = " ";
    int fract_digits = 2;
    std::string positive_symbol = "";
    std::string negative_symbol = "-";

    NumberConfig() = default;
    NumberConfig(std::string decimal, std::string thousand, int fract,
                 std::string positive, std::string negative)
        : decimal_symbol(std::move(decimal)), thousand_separator(std::move(thousand)),
          fract_digits(fract), positive_symbol(std::move(positive)),
          negative_symbol(std::move(negative)) {}
    NumberConfig(NumberConfig&&) noexcept = default;
    NumberConfig& operator=(NumberConfig&&) noexcept = default;
    NumberConfig(const NumberConfig&) = default;
    NumberConfig& operator=(const NumberConfig&) = default;
};

struct DateTimeConfig {
    std::string long_time = "%H:%M:%S";
    std::string short_time = "%H:%M";
    std::string long_date = "%B %d, %Y";
    std::string short_date = "%m/%d/%Y";
    std::string long_date_time = "%B %d, %Y %H:%M:%S";
    std::string short_date_time = "%m/%d/%Y %H:%M";

    DateTimeConfig() = default;
    DateTimeConfig(DateTimeConfig&&) noexcept = default;
    DateTimeConfig& operator=(DateTimeConfig&&) noexcept = default;
    DateTimeConfig(const DateTimeConfig&) = default;
    DateTimeConfig& operator=(const DateTimeConfig&) = default;
};

struct FormatConfig {
    CurrencyConfig currency;
    NumberConfig number;
    DateTimeConfig date_time;
    std::vector<std::string> short_month_names = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    std::vector<std::string> long_month_names = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };
    std::vector<std::string> short_day_names = {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };
    std::vector<std::string> long_day_names = {
        "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
        "Friday", "Saturday"
    };

    FormatConfig() = default;
    FormatConfig(FormatConfig&&) noexcept = default;
    FormatConfig& operator=(FormatConfig&&) noexcept = default;
    FormatConfig(const FormatConfig&) = default;
    FormatConfig& operator=(const FormatConfig&) = default;
};

/// Internationalization library supporting translation, plural forms,
/// number/currency/date formatting, and positional/named/formatted interpolation.
///
/// \warning **This class is NOT thread-safe, not even for `const` methods.**
/// The following members are `mutable` and written on every call:
///   - `formatCache_` / `translationCache_` — memoization caches, written on cache miss
///   - `interpolateBuf_` / `interpolateBuf2_` / `extendedParamsBuf_` — scratch buffers,
///     written on every interpolation call
///
/// Calling any method (including `tr()`, `trPlural()`, `format*()`) on a shared
/// instance from multiple threads is a data race and will cause UB — typically
/// segfault from corrupted `unordered_map` bucket pointers, not merely stale data.
///
/// Valid usage patterns:
///   1. Create one `I18N` instance per thread.
///   2. Share a single instance across threads, but guard every call (including
///      `tr()`) with an external `std::mutex`.
///
/// `reset()`, `loadLocale()`, `mergeLocale()`, `load()`, `setLocale()`,
/// `setFallbackLocale()`, and `configure()` are non-const mutators and likewise
/// require external synchronization if the instance is shared.
class I18N {
public:
    I18N();
    ~I18N() = default;

    // Disable copying (use only move semantics)
    I18N(const I18N&) = delete;
    I18N& operator=(const I18N&) = delete;
    // Enable moving
    I18N(I18N&&) noexcept = default;
    I18N& operator=(I18N&&) noexcept = default;

    void loadLocaleFromFile(std::string_view filePath);
    void loadLocale(std::string_view locale, std::string_view filePath);

    // Merge new translations on top of existing locale data
    void mergeLocaleFromFile(std::string_view filePath);
    void mergeLocale(std::string_view locale, std::string_view filePath);

    void load(const json& data);

    void setLocale(std::string_view locale);
    void setLocale(const std::vector<std::string>& locales);

    void setFallbackLocale(std::string_view locale);

    std::string getLocale() const noexcept;
    const std::vector<std::string>& getLocales() const noexcept;

    std::string getFallbackLocale() const noexcept;

    std::string translate(std::string_view key, const json& params = json::object());
    
    std::string tr(std::string_view key) const;
    std::string tr(std::string_view key, std::span<const std::string> params) const;
    std::string tr(std::string_view key, std::initializer_list<std::string> params) const;
    
    template<typename... Args>
    std::string trv(std::string_view key, const Args&... args) const {
        return tr(key, argsToStrings(args...));
    }
    
    std::string trPlural(std::string_view key, int count) const;
    std::string trPlural(std::string_view key, int count, std::span<const std::string> params) const;
    std::string trPlural(std::string_view key, int count, std::initializer_list<std::string> params) const;
    
    template<typename... Args>
    std::string trPluralv(std::string_view key, int count, const Args&... args) const {
        return trPlural(key, count, argsToStrings(args...));
    }
    
    bool keyExists(std::string_view key) const noexcept;

    void configure(const json& formats);
    const FormatConfig& getConfig() const noexcept;

    // Formatting
    std::string formatNumber(double number) const;
    std::string formatPrice(double amount) const;
    std::string formatDate(std::string_view pattern = "", const std::tm* date = nullptr) const;

    void reset();

    size_t formatCacheSize() const noexcept;
    size_t translationCacheSize() const noexcept;

private:
    std::vector<std::string> locales;
    std::string fallbackLocale;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string, StringHash, StringEqual>, StringHash, StringEqual> localesData;
    std::unordered_map<std::string, FormatConfig, StringHash, StringEqual> formatConfigs;
    FormatConfig defaultConfig;
    FormatConfig baselineConfig_;
    // NOTE: the following mutable members are written from const methods —
    // see class-level thread-safety \warning above. Do not assume const methods
    // are safe to call on a shared instance from multiple threads.
    mutable std::unordered_map<std::string, std::string, StringHash, StringEqual> formatCache_;
    mutable std::unordered_map<std::string, std::string, StringHash, StringEqual> translationCache_;
    mutable std::string interpolateBuf_;
    mutable std::string interpolateBuf2_;
    mutable std::vector<std::string> extendedParamsBuf_;

    // Helper functions
    void clearFormatCache();
    void clearTranslationCache();
    const std::string* getTranslationData(std::string_view key, std::string_view locale) const;
    
    std::string interpolate(std::string_view text, const json& params) const;

    std::string interpolateArray(std::string_view text, std::span<const std::string> params) const;

    std::string getPluralForm(std::string_view locale, int count) const;

    void flattenJson(const std::string& prefix, const json& node, std::unordered_map<std::string, std::string, StringHash, StringEqual>& flatMap);
    void flattenJson(const std::string& prefix, json&& node, std::unordered_map<std::string, std::string, StringHash, StringEqual>& flatMap);

    std::vector<std::string> getLocaleAncestry(std::string_view locale) const;
    
    std::vector<std::string> getFallbacks(const std::vector<std::string>& locales) const;
    
    std::string getLocaleRoot(std::string_view locale) const;
    
    // Formatting helpers
    std::string formatNumberWithConfig(double number, const NumberConfig& config) const;
    std::string formatPriceWithConfig(double amount, const CurrencyConfig& config) const;
    std::string formatDateWithConfig(std::string_view pattern, const std::tm* date, const DateTimeConfig& config) const;
    
    std::string separateThousand(std::string_view amount, std::string_view separator) const;

    template<typename T>
    std::string toString(const T& value) const {
        if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view> || std::is_same_v<T, const char*>) {
            return std::string(value);
        } else if constexpr (std::is_same_v<T, bool>) {
            return value ? "true" : "false";
        } else if constexpr (std::is_arithmetic_v<T>) {
            return std::to_string(value);
        } else {
            // For other types, add conversion as needed
            std::ostringstream oss;
            oss << value;
            return oss.str();
        }
    }
    
    template<typename... Args>
    std::vector<std::string> argsToStrings(const Args&... args) const {
        std::vector<std::string> result;
        result.reserve(sizeof...(args));
        (result.push_back(toString(args)), ...);
        return result;
    }

    std::vector<std::string> argsToStrings() const {
        return {};
    }
};

} // namespace i18n

#endif // I18NCPP_H

