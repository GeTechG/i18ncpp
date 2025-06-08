#pragma once

#ifndef I18NCPP_H
#define I18NCPP_H

#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <regex>
#include <ctime>
#include <optional>
#include <memory>
#include <initializer_list>
#include <nlohmann/json.hpp>
#include <sstream>
#include <tuple>
#include <type_traits>
#include <algorithm>
#include <cctype>

namespace i18n {

using json = nlohmann::json;

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

// Main library class
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

    void load(const json& data);

    void setLocale(std::string_view locale);
    void setLocale(const std::vector<std::string>& locales);

    void setFallbackLocale(std::string_view locale);

    std::string getLocale() const noexcept;
    const std::vector<std::string>& getLocales() const noexcept;

    std::string getFallbackLocale() const noexcept;

    std::string translate(std::string_view key, const json& params = json::object());
    
    std::string tr(std::string_view key) const;
    std::string tr(std::string_view key, const std::string& param1) const;
    std::string tr(std::string_view key, const std::string& param1, const std::string& param2) const;
    std::string tr(std::string_view key, const std::string& param1, const std::string& param2, const std::string& param3) const;
    std::string tr(std::string_view key, std::initializer_list<std::string> params) const;
    std::string tr(std::string_view key, const std::vector<std::string>& params) const;
    
    template<typename... Args>
    std::string trv(std::string_view key, const Args&... args) const {
        return tr(key, argsToStrings(args...));
    }
    
    std::string trPlural(std::string_view key, int count) const;
    std::string trPlural(std::string_view key, int count, const std::string& param1) const;
    std::string trPlural(std::string_view key, int count, const std::string& param1, const std::string& param2) const;
    std::string trPlural(std::string_view key, int count, const std::vector<std::string>& params) const;
    
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

private:
    std::vector<std::string> locales;
    std::string fallbackLocale;
    std::unordered_map<std::string, json> localesData;
    std::unordered_map<std::string, FormatConfig> formatConfigs;
    FormatConfig defaultConfig;

    // Helper functions
    json* getTranslationData(std::string_view key, std::string_view locale);
    const json* getTranslationData(std::string_view key, std::string_view locale) const;
    std::string localizedTranslate(std::string_view key, std::string_view locale, const json& params = json::object()) const;
    
    std::string interpolate(std::string_view text, const json& params) const;
    
    std::string interpolateArray(std::string_view text, const std::vector<std::string>& params) const;
    
    std::string handlePlural(const json& pluralData, std::string_view locale, const json& params) const;
    std::string handlePluralWithArray(const json& pluralData, std::string_view locale, int count, const std::vector<std::string>& params) const;
    std::string getPluralForm(std::string_view locale, int count) const;
    
    std::string handleVariant(const json& variantData, const json& params) const;
    
    std::vector<std::string> dotSplit(std::string_view str) const;
    
    std::vector<std::string> getLocaleAncestry(std::string_view locale) const;
    
    std::vector<std::string> getFallbacks(const std::vector<std::string>& locales) const;
    
    std::string getLocaleRoot(std::string_view locale) const;
    
    // Formatting helpers
    std::string formatNumberWithConfig(double number, const NumberConfig& config) const;
    std::string formatPriceWithConfig(double amount, const CurrencyConfig& config) const;
    std::string formatDateWithConfig(std::string_view pattern, const std::tm* date, const DateTimeConfig& config) const;
    
    std::string separateThousand(std::string_view amount, std::string_view separator) const;

    static const std::regex fieldPattern;      // %{key}
    static const std::regex valuePattern;      // %<key>.fmt
    static const std::regex indexPattern;      // {index}
    static const std::regex emptyBracePattern; // {}

    template<typename T>
    std::string toString(const T& value) const {
        if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view> || std::is_same_v<T, const char*>) {
            return std::string(value);
        } else if constexpr (std::is_arithmetic_v<T>) {
            return std::to_string(value);
        } else if constexpr (std::is_same_v<T, bool>) {
            return value ? "true" : "false";
        } else {
            // For other types, add conversion as needed
            std::ostringstream oss;
            oss << value;
            return oss.str();
        }
    }
    
    template<typename T>
    std::vector<std::string> argsToStrings(const T& arg) const {
        return { toString(arg) };
    }
    
    template<typename T, typename... Args>
    std::vector<std::string> argsToStrings(const T& first, const Args&... rest) const {
        auto result = argsToStrings(rest...);
        result.insert(result.begin(), toString(first));
        return result;
    }
    
    std::vector<std::string> argsToStrings() const {
        return {};
    }
};

} // namespace i18n

#endif // I18NCPP_H

