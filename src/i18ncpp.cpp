#include <iostream>
#include "i18ncpp.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <regex>
#include <set>
#include <functional>
#include <charconv>

namespace i18n {

const std::regex I18N::fieldPattern(R"(([^%]|^)%\{([\w\.]+)\})");
const std::regex I18N::valuePattern(R"(([^%]|^)%<([\w\.]+)>\.([\w]))");
const std::regex I18N::indexPattern(R"(\{(\d+)\})");        // {0}, {1}, {2}, ...
const std::regex I18N::emptyBracePattern(R"(\{\})");        // {}

I18N::I18N() {
    reset();
}

void I18N::loadLocale(std::string_view locale, std::string_view filePath) {
    std::ifstream fileStream(filePath.data());
    if (!fileStream.is_open()) {
        throw I18NError("Failed to open locale file: " + std::string(filePath));
    }

    try {
        json data = json::parse(fileStream);
        std::string localeStr(locale);
        localesData[localeStr] = std::move(data);
        
        // Extract formatting configuration if present
        if (localesData[localeStr].contains("_formats") && localesData[localeStr]["_formats"].is_object()) {
            configure(localesData[localeStr]["_formats"]);
            formatConfigs[localeStr] = defaultConfig;
            // Remove _formats from locale data
            localesData[localeStr].erase("_formats");
        }
    } catch (const json::parse_error& e) {
        throw I18NError("Failed to parse JSON from file: " + std::string(filePath) + " - " + e.what());
    }
}

void I18N::loadLocaleFromFile(std::string_view filePath) {
    std::string_view filename = filePath;

    size_t lastSlash = filename.find_last_of("/\\");
    if (lastSlash != std::string_view::npos) {
        filename = filename.substr(lastSlash + 1);
    }

    size_t extensionPos = filename.find_last_of('.');
    if (extensionPos != std::string_view::npos) {
        filename = filename.substr(0, extensionPos);
    }

    if (filename.empty()) {
        throw I18NError("Cannot extract locale from file name: " + std::string(filePath));
    }

    loadLocale(filename, filePath);
}

void I18N::load(const json& data) {
    std::function<void(const std::string&, const json&)> recursiveLoad;
    recursiveLoad = [this, &recursiveLoad](const std::string& currentContext, const json& data) {
    
        if (data.contains("_formats") && data["_formats"].is_object()) {
            if (!currentContext.empty()) {
                configure(data["_formats"]);
                formatConfigs[currentContext] = defaultConfig;
            }
        }
        
        // Process translations
        for (auto it = data.begin(); it != data.end(); ++it) {
            if (it.key() == "_formats") continue;
            
            std::string composedKey = currentContext.empty() ? it.key() : currentContext + "." + it.key();
            
            if (it.value().is_string() || it.value().is_array()) {
                localesData[composedKey] = it.value();
            } else if (it.value().is_object()) {
                recursiveLoad(composedKey, it.value());
            }
        }
    };
    
    recursiveLoad("", data);
}

void I18N::setLocale(std::string_view locale) {
    std::string localeStr(locale);
    locales.clear();
    locales.push_back(localeStr);
    
    if (formatConfigs.find(localeStr) != formatConfigs.end()) {
        defaultConfig = formatConfigs[localeStr];
    }
}

void I18N::setLocale(const std::vector<std::string>& newLocales) {    
    locales = newLocales;
    
    if (!locales.empty() && formatConfigs.find(locales[0]) != formatConfigs.end()) {
        defaultConfig = formatConfigs[locales[0]];
    }
}

void I18N::setFallbackLocale(std::string_view locale) {
    fallbackLocale = std::string(locale);
}

std::string I18N::getLocale() const noexcept {
    return locales.empty() ? "" : locales[0];
}

const std::vector<std::string>& I18N::getLocales() const noexcept {
    return locales;
}

std::string I18N::getFallbackLocale() const noexcept {
    return fallbackLocale;
}

std::vector<std::string> I18N::dotSplit(std::string_view str) const {
    std::vector<std::string> result;
    result.reserve(4); // Reserve memory for optimization (typical number of segments)
    
    size_t start = 0;
    size_t end = str.find('.');
    
    while (end != std::string_view::npos) {
        result.emplace_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find('.', start);
    }
    
    if (start < str.size()) {
        result.emplace_back(str.substr(start));
    }
    
    return result;
}

json* I18N::getTranslationData(std::string_view key, std::string_view locale) {
    if (locale.empty()) {
        return nullptr;
    }
    
    std::string localeStr(locale);
    auto it = localesData.find(localeStr);
    if (it == localesData.end()) {
        return nullptr;
    }
    
    std::vector<std::string> path = dotSplit(key);
    json* node = &it->second;
    
    for (const auto& part : path) {
        if (!node->is_object() || !node->contains(part)) {
            return nullptr;
        }
        node = &(*node)[part];
    }
    
    return node;
}

const json* I18N::getTranslationData(std::string_view key, std::string_view locale) const {
    if (locale.empty()) {
        return nullptr;
    }
    
    std::string localeStr(locale);
    auto it = localesData.find(localeStr);
    if (it == localesData.end()) {
        return nullptr;
    }
    
    std::vector<std::string> path = dotSplit(key);
    const json* node = &it->second;
    
    for (const auto& part : path) {
        if (!node->is_object() || !node->contains(part)) {
            return nullptr;
        }
        node = &(*node)[part];
    }
    
    return node;
}

std::string I18N::interpolate(std::string_view text, const json& params) const {
    if (!params.is_object() || text.empty()) {
        return std::string(text);
    }
    
    std::string result(text);
    
    // Process templates like %{key}
    std::string newResult;
    newResult.reserve(result.size() * 1.5); // Reserve extra memory for optimization
    
    std::string::const_iterator searchStart(result.cbegin());
    std::smatch match;
    size_t lastPos = 0;
    
    while (std::regex_search(searchStart, result.cend(), match, fieldPattern)) {
        // Add text before match
        size_t startPos = match.position() + (searchStart - result.cbegin());
        newResult.append(result, lastPos, startPos - lastPos);
        
        // Add previous character
        std::string prev = match[1].str();
        newResult.append(prev);
        
        // Get key and replace with value if it exists
        std::string key = match[2].str();
        if (params.contains(key)) {
            json value = params[key];
            if (value.is_string()) {
                newResult.append(value.get<std::string>());
            } else if (value.is_number()) {
                newResult.append(value.dump());
            } else if (value.is_boolean()) {
                newResult.append(value.get<bool>() ? "true" : "false");
            } else {
                // For other types, use JSON representation
                newResult.append(value.dump());
            }
        } else {
            // If key not found, keep template as is
            newResult.append("%{").append(key).append("}");
        }
        
        // Update search position and last position
        searchStart += match.position() + match.length();
        lastPos = startPos + match.length();
    }
    
    // Add remaining text
    newResult.append(result, lastPos, std::string::npos);
    
    // Process templates like %<key>.fmt
    result = std::move(newResult);
    newResult.clear();
    newResult.reserve(result.size() * 1.5);
    
    lastPos = 0;
    searchStart = result.cbegin();
    
    while (std::regex_search(searchStart, result.cend(), match, valuePattern)) {
        // Add text before match
        size_t startPos = match.position() + (searchStart - result.cbegin());
        newResult.append(result, lastPos, startPos - lastPos);
        
        // Add previous character
        std::string prev = match[1].str();
        newResult.append(prev);
        
        // Get key and format
        std::string key = match[2].str();
        std::string format = match[3].str();
        
        if (params.contains(key)) {
            json value = params[key];
            
            std::ostringstream oss;
            if (format == "d" || format == "i") {
                // Integer format
                oss << static_cast<int>(value.is_number() ? value.get<double>() : 0);
            } else if (format == "f") {
                // Floating point format
                oss << (value.is_number() ? value.get<double>() : 0.0);
            } else if (format == "s") {
                // String format
                oss << (value.is_string() ? value.get<std::string>() : value.dump());
            } else {
                // Other formats
                oss << value.dump();
            }
            
            newResult.append(oss.str());
        } else {
            // If key not found, keep template as is
            newResult.append("%<").append(key).append(">.").append(format);
        }
        
        // Update search position and last position
        searchStart += match.position() + match.length();
        lastPos = startPos + match.length();
    }
    
    // Add remaining text
    newResult.append(result, lastPos, std::string::npos);
    
    return newResult;
}

std::string I18N::interpolateArray(std::string_view text, const std::vector<std::string>& params) const {
    if (params.empty() || text.empty()) {
        return std::string(text);
    }
    
    std::string result(text);
    
    // First replace numbered placeholders {0}, {1}, {2}, ...
    std::string::const_iterator searchStart(result.cbegin());
    std::smatch match;
    
    std::string newResult;
    newResult.reserve(result.size() * 1.5); // reserve extra memory
    
    size_t lastPos = 0;
    
    while (std::regex_search(searchStart, result.cend(), match, indexPattern)) {
        // Add text before match
        size_t startPos = match.position() + (searchStart - result.cbegin());
        newResult.append(result, lastPos, startPos - lastPos);
        
        // Get parameter index
        int index = std::stoi(match[1].str());
        
        // Substitute parameter with corresponding index if it exists
        if (index >= 0 && index < static_cast<int>(params.size())) {
            newResult.append(params[index]);
        } else {
            // If parameter with such index not found, keep placeholder
            newResult.append(match[0].str());
        }
        
        // Update positions for next search
        searchStart += match.position() + match.length();
        lastPos = startPos + match.length();
    }
    
    // Add remaining text
    newResult.append(result, lastPos, std::string::npos);
    result = std::move(newResult);
    
    // Now replace unnumbered placeholders {} with parameters in order
    searchStart = result.cbegin();
    newResult.clear();
    newResult.reserve(result.size() * 1.5);
    
    lastPos = 0;
    size_t paramIndex = 0; // Index of current parameter
    
    while (std::regex_search(searchStart, result.cend(), match, emptyBracePattern)) {
        // Add text before match
        size_t startPos = match.position() + (searchStart - result.cbegin());
        newResult.append(result, lastPos, startPos - lastPos);
        
        // If there are available parameters, substitute next one
        if (paramIndex < params.size()) {
            newResult.append(params[paramIndex++]);
        } else {
            // If parameters are exhausted, keep placeholder
            newResult.append("{}");
        }
        
        // Update positions for next search
        searchStart += match.position() + match.length();
        lastPos = startPos + match.length();
    }
    
    // Add remaining text
    newResult.append(result, lastPos, std::string::npos);
    
    return newResult;
}

std::string I18N::getLocaleRoot(std::string_view locale) const {
    size_t dashPos = locale.find('-');
    if (dashPos != std::string_view::npos) {
        return std::string(locale.substr(0, dashPos));
    }
    return std::string(locale);
}

std::vector<std::string> I18N::getLocaleAncestry(std::string_view locale) const {
    std::vector<std::string> result;
    result.reserve(3); // Usually no more than 2-3 ancestors (en, en-US, en-US-NY)
    
    std::string current;
    current.reserve(locale.size());
    
    for (size_t i = 0; i < locale.size(); ++i) {
        if (locale[i] == '-') {
            result.push_back(current);
        }
        current.push_back(locale[i]);
    }
    
    result.push_back(current);
    
    // Reverse the list so that the most specific locales are first
    std::reverse(result.begin(), result.end());
    
    return result;
}

std::vector<std::string> I18N::getFallbacks(const std::vector<std::string>& locales) const {
    std::unordered_map<std::string, bool> seen;
    std::vector<std::string> result;
    result.reserve(locales.size() * 2 + 1); // Reserve extra memory
    
    for (const auto& locale : locales) {
        auto ancestry = getLocaleAncestry(locale);
        for (const auto& ancestor : ancestry) {
            if (!seen[ancestor]) {
                seen[ancestor] = true;
                result.push_back(ancestor);
            }
        }
    }
    
    // Add fallback locale if it's not empty and not already in the list
    if (!fallbackLocale.empty() && !seen[fallbackLocale]) {
        result.push_back(fallbackLocale);
    }
    
    return result;
}

std::string I18N::getPluralForm(std::string_view locale, int count) const {
    std::string root = getLocaleRoot(locale);
    
    // Implement plural rules for different languages
    static const std::unordered_map<std::string, int> localeRules = {
        // English and similar languages (1)
        {"en", 1}, {"de", 1}, {"nl", 1}, {"sv", 1}, {"da", 1}, {"no", 1}, {"nb", 1}, 
        {"nn", 1}, {"fo", 1}, {"es", 1}, {"pt", 1}, {"it", 1}, {"bg", 1}, {"el", 1}, 
        {"fi", 1}, {"et", 1}, {"he", 1}, {"eo", 1},
        
        // Russian and similar Slavic languages (5)
        {"ru", 5}, {"uk", 5}, {"be", 5}, {"hr", 5}, {"sr", 5}, {"bs", 5}, {"sh", 5},
        
        // Polish (21)
        {"pl", 21},
        
        // Czech and Slovak (7)
        {"cs", 7}, {"sk", 7},
        
        // French and similar (9)
        {"fr", 9}, {"ff", 9}, {"kab", 9},
        
        // Arabic (3)
        {"ar", 3}
    };
    
    auto ruleIt = localeRules.find(root);
    int rule = (ruleIt != localeRules.end()) ? ruleIt->second : 1; // default to English-like
    
    switch (rule) {
        case 1: // English and similar
            return (count == 1) ? "one" : "other";
            
        case 5: // Russian and similar
            if (count % 10 == 1 && count % 100 != 11) {
                return "one";
            } else if (count % 10 >= 2 && count % 10 <= 4 && (count % 100 < 12 || count % 100 > 14)) {
                return "few";
            } else if (count % 10 == 0 || (count % 10 >= 5 && count % 10 <= 9) || (count % 100 >= 11 && count % 100 <= 14)) {
                return "many";
            } else {
                return "other";
            }
            
        case 21: // Polish
            if (count == 1) {
                return "one";
            } else if (count % 10 >= 2 && count % 10 <= 4 && (count % 100 < 12 || count % 100 > 14)) {
                return "few";
            } else {
                return "many";
            }
            
        case 7: // Czech and Slovak
            if (count == 1) {
                return "one";
            } else if (count >= 2 && count <= 4) {
                return "few";
            } else {
                return "other";
            }
            
        case 9: // French and similar
            return count < 2 ? "one" : "other";
            
        case 3: // Arabic
            if (count == 0) {
                return "zero";
            } else if (count == 1) {
                return "one";
            } else if (count == 2) {
                return "two";
            } else if (count % 100 >= 3 && count % 100 <= 10) {
                return "few";
            } else if (count % 100 >= 11 && count % 100 <= 99) {
                return "many";
            } else {
                return "other";
            }
            
        default: // Default to English-like
            return count == 1 ? "one" : "other";
    }
}

std::string I18N::handlePlural(const json& pluralData, std::string_view locale, const json& params) const {
    if (!pluralData.is_object()) {
        return "[plural: data not object]";
    }
    
    // Use count parameter to select plural form
    int count = 1; // Default
    if (params.contains("count") && params["count"].is_number()) {
        count = params["count"].get<int>();
    }
    
    // Get plural form for the given count and locale
    std::string pluralForm = getPluralForm(locale, count);
    
    // Check for form in data
    if (pluralData.contains(pluralForm)) {
        return pluralData[pluralForm].get<std::string>();
    } else if (pluralData.contains("other")) {
        // Use 'other' as fallback
        return pluralData["other"].get<std::string>();
    } else if (pluralData.contains(std::to_string(count))) {
        // Try to find exact match for the number
        return pluralData[std::to_string(count)].get<std::string>();
    }
    
    return "[plural: missing form]";
}

std::string I18N::handlePluralWithArray(const json& pluralData, std::string_view locale, int count, const std::vector<std::string>& params) const {
    if (!pluralData.is_object()) {
        return "[plural: data not object]";
    }
    
    std::string pluralForm = getPluralForm(locale, count);
    
    std::string template_str;
    
    if (pluralData.contains(pluralForm)) {
        template_str = pluralData[pluralForm].get<std::string>();
    } else if (pluralData.contains("other")) {
        template_str = pluralData["other"].get<std::string>();
    } else if (pluralData.contains(std::to_string(count))) {
        template_str = pluralData[std::to_string(count)].get<std::string>();
    } else {
        return "[plural: missing form]";
    }
    
    std::vector<std::string> extendedParams;
    extendedParams.reserve(params.size() + 1);
    extendedParams.push_back(std::to_string(count));
    extendedParams.insert(extendedParams.end(), params.begin(), params.end());
    
    return interpolateArray(template_str, extendedParams);
}

std::string I18N::handleVariant(const json& variantData, const json& params) const {
    if (!variantData.is_object()) {
        return "[variant: data not object]";
    }
    
    // Find key in parameters that matches one of the keys in variantData
    for (auto it = params.begin(); it != params.end(); ++it) {
        if (it.value().is_string()) {
            const std::string& value = it.value().get<std::string>();
            if (variantData.contains(value)) {
                return variantData[value].get<std::string>();
            }
        }
    }
    
    if (variantData.contains("other")) {
        return variantData["other"].get<std::string>();
    }
    
    return "[variant: no match]";
}

std::string I18N::localizedTranslate(std::string_view key, std::string_view locale, const json& params) const {
    const json* translationData = getTranslationData(key, locale);
    if (!translationData) {
        return ""; // Key not found
    }
    
    if (translationData->is_string()) {
        return interpolate(translationData->get<std::string>(), params);
    } else if (translationData->is_object()) {
        if (params.contains("count")) {

            return interpolate(handlePlural(*translationData, locale, params), params);
        } else {

            return interpolate(handleVariant(*translationData, params), params);
        }
    } else if (translationData->is_array()) {
        return translationData->dump();
    }
    
    return "[unsupported translation type]";
}

std::string I18N::translate(std::string_view key, const json& params) {
    if (key.empty()) {
        return "";
    }
    
    std::vector<std::string> searchLocales = locales;
    
    if (params.contains("locale") && params["locale"].is_string()) {
        const std::string& paramLocale = params["locale"].get<std::string>();
        searchLocales.insert(searchLocales.begin(), paramLocale);
    }
    
    std::vector<std::string> fallbacks = getFallbacks(searchLocales);
    
    for (const auto& loc : fallbacks) {
        std::string translation = localizedTranslate(key, loc, params);
        if (!translation.empty()) {
            return translation;
        }
    }
    
    if (params.contains("default") && params["default"].is_string()) {
        return interpolate(params["default"].get<std::string>(), params);
    }
    
    return std::string(key);
}

std::string I18N::tr(std::string_view key) const {
    std::vector<std::string> empty;
    return tr(key, empty);
}

std::string I18N::tr(std::string_view key, const std::string& param1) const {
    return tr(key, std::vector<std::string>{param1});
}

std::string I18N::tr(std::string_view key, const std::string& param1, const std::string& param2) const {
    return tr(key, std::vector<std::string>{param1, param2});
}

std::string I18N::tr(std::string_view key, const std::string& param1, const std::string& param2, const std::string& param3) const {
    return tr(key, std::vector<std::string>{param1, param2, param3});
}

std::string I18N::tr(std::string_view key, std::initializer_list<std::string> params) const {
    return tr(key, std::vector<std::string>(params));
}

std::string I18N::tr(std::string_view key, const std::vector<std::string>& params) const {
    if (key.empty()) {
        return "";
    }
    
    std::vector<std::string> fallbacks = getFallbacks(locales);
    
    for (const auto& loc : fallbacks) {
        const json* translationData = getTranslationData(key, loc);
        if (!translationData) continue;
        
        if (translationData->is_string()) {
            return interpolateArray(translationData->get<std::string>(), params);
        } else if (translationData->is_object()) {
            if (translationData->contains("other")) {
                return interpolateArray((*translationData)["other"].get<std::string>(), params);
            } else if (!translationData->empty()) {
                auto it = translationData->begin();
                return interpolateArray(it.value().get<std::string>(), params);
            }
        }
    }
    
    return std::string(key);
}

std::string I18N::trPlural(std::string_view key, int count) const {
    std::vector<std::string> empty;
    return trPlural(key, count, empty);
}

std::string I18N::trPlural(std::string_view key, int count, const std::string& param1) const {
    return trPlural(key, count, std::vector<std::string>{param1});
}

std::string I18N::trPlural(std::string_view key, int count, const std::string& param1, const std::string& param2) const {
    return trPlural(key, count, std::vector<std::string>{param1, param2});
}

std::string I18N::trPlural(std::string_view key, int count, const std::vector<std::string>& params) const {
    if (key.empty()) {
        return "";
    }
    
    std::vector<std::string> fallbacks = getFallbacks(locales);
    
    for (const auto& loc : fallbacks) {
        const json* translationData = getTranslationData(key, loc);
        if (!translationData) continue;
        
        if (translationData->is_object()) {
            return handlePluralWithArray(*translationData, loc, count, params);
        } else if (translationData->is_string()) {
            std::vector<std::string> extendedParams;
            extendedParams.reserve(params.size() + 1);
            extendedParams.push_back(std::to_string(count));
            extendedParams.insert(extendedParams.end(), params.begin(), params.end());
            
            return interpolateArray(translationData->get<std::string>(), extendedParams);
        }
    }
    
    return std::string(key);
}

bool I18N::keyExists(std::string_view key) const noexcept {
    if (locales.empty()) {
        return false;
    }
    
    try {
        std::vector<std::string> fallbacks = getFallbacks(locales);
        
        for (const auto& loc : fallbacks) {
            if (getTranslationData(key, loc) != nullptr) {
                return true;
            }
        }
    } catch (...) {
        return false;
    }
    
    return false;
}

std::string I18N::separateThousand(std::string_view amount, std::string_view separator) const {
    std::string result(amount);
    
    size_t decimalPos = result.find('.');
    std::string integerPart = (decimalPos != std::string::npos) ? result.substr(0, decimalPos) : result;
    std::string fractionalPart = (decimalPos != std::string::npos) ? result.substr(decimalPos) : "";
    
    bool isNegative = !integerPart.empty() && integerPart[0] == '-';
    
    if (isNegative) {
        integerPart = integerPart.substr(1);
    }
    
    int insertPos = static_cast<int>(integerPart.length()) - 3;
    while (insertPos > 0) {
        integerPart.insert(insertPos, separator);
        insertPos -= 3;
    }
    
    result = (isNegative ? "-" : "") + integerPart + fractionalPart;
    
    return result;
}

std::string I18N::formatNumberWithConfig(double number, const NumberConfig& config) const {
    std::string result;
    result.reserve(30); // Reserve memory for optimization
    
    bool isNegative = number < 0;
    double absNumber = std::abs(number);
    
    double rounded = std::round(absNumber * std::pow(10, config.fract_digits)) / std::pow(10, config.fract_digits);
    
    long long integerPart = static_cast<long long>(rounded);
    double fractionalPart = rounded - integerPart;
    
    std::string integerStr = std::to_string(integerPart);
    integerStr = separateThousand(integerStr, config.thousand_separator);
    
    result.append(integerStr);
    
    if (config.fract_digits > 0) {
        result.append(config.decimal_symbol);
        
        long long scaledFract = static_cast<long long>(std::round(fractionalPart * std::pow(10, config.fract_digits)));
        std::string fractionalStr = std::to_string(scaledFract);
        

        if (fractionalStr.length() < static_cast<size_t>(config.fract_digits)) {
            result.append(config.fract_digits - fractionalStr.length(), '0');
        }
        
        result.append(fractionalStr);
    }
    
    if (isNegative) {
        return config.negative_symbol + result;
    } else if (!config.positive_symbol.empty()) {
        return config.positive_symbol + result;
    }
    
    return result;
}

std::string I18N::formatPriceWithConfig(double amount, const CurrencyConfig& config) const {
    std::string formattedNumber = formatNumberWithConfig(amount, 
        NumberConfig{
            config.decimal_symbol,
            config.thousand_separator,
            config.fract_digits,
            config.positive_symbol,
            config.negative_symbol
        }
    );
    
    std::string pattern = amount < 0 ? config.negative_format : config.positive_format;
    
    std::string result;
    result.reserve(pattern.size() + formattedNumber.size() + config.symbol.size());
    
    size_t pos = 0;
    size_t foundPos;
    
    while (pos < pattern.size()) {
        foundPos = pattern.find('%', pos);
        if (foundPos == std::string::npos) {
            result.append(pattern, pos, std::string::npos);
            break;
        }
        
        result.append(pattern, pos, foundPos - pos);
        
        if (foundPos + 1 < pattern.size()) {
            char formatChar = pattern[foundPos + 1];
            switch (formatChar) {
                case 'p': // %p - Empty string as sign is already included in formattedNumber
                    break;
                case 'q': // %q - Formatted number
                    result.append(formattedNumber);
                    break;
                case 'c': // %c - Currency symbol
                    result.append(config.symbol);
                    break;
                default: // Unknown format - keep as is
                    result.push_back('%');
                    result.push_back(formatChar);
                    break;
            }
            pos = foundPos + 2; // Skip % and format character
        } else {
            // % at the end of the string
            result.push_back('%');
            pos = foundPos + 1;
        }
    }
    
    return result;
}

std::string I18N::formatDateWithConfig(std::string_view pattern, const std::tm* date, const DateTimeConfig& config) const {
    std::tm timeinfo;
    if (date) {
        timeinfo = *date;
    } else {
        std::time_t now = std::time(nullptr);
        localtime_s(&timeinfo, &now);
    }
    
    // Determine format pattern
    std::string formatPattern;
    
    // If pattern is empty, use ISO 8601
    if (pattern.empty()) {
        formatPattern = "%Y-%m-%dT%H:%M:%S";
    } else if (pattern == "long_time") {
        formatPattern = config.long_time;
    } else if (pattern == "short_time") {
        formatPattern = config.short_time;
    } else if (pattern == "long_date") {
        formatPattern = config.long_date;
    } else if (pattern == "short_date") {
        formatPattern = config.short_date;
    } else if (pattern == "long_date_time") {
        formatPattern = config.long_date_time;
    } else if (pattern == "short_date_time") {
        formatPattern = config.short_date_time;
    } else {
        formatPattern = std::string(pattern);
    }
    
    std::string result;
    result.reserve(formatPattern.size() * 2);

    size_t pos = 0;
    size_t foundPos;
    
    while (pos < formatPattern.size()) {
        foundPos = formatPattern.find('%', pos);
        if (foundPos == std::string::npos) {
            // Add remaining part of the pattern
            result.append(formatPattern, pos, std::string::npos);
            break;
        }
        
        // Add text before %
        result.append(formatPattern, pos, foundPos - pos);
        
        // Skip % and check next character
        if (foundPos + 1 < formatPattern.size()) {
            char formatChar = formatPattern[foundPos + 1];
            
            // Format according to specifier
            switch (formatChar) {
                case 'H': // %H - Hour (00-23)
                    result.append(std::to_string(timeinfo.tm_hour / 10)).append(std::to_string(timeinfo.tm_hour % 10));
                    break;
                case 'M': // %M - Minute (00-59)
                case 'i': // %i - Alias for minute
                    result.append(std::to_string(timeinfo.tm_min / 10)).append(std::to_string(timeinfo.tm_min % 10));
                    break;
                case 'S': // %S - Second (00-59)
                case 's': // %s - Alias for second
                    result.append(std::to_string(timeinfo.tm_sec / 10)).append(std::to_string(timeinfo.tm_sec % 10));
                    break;
                case 'd': // %d - Day of the month (01-31)
                    result.append(std::to_string(timeinfo.tm_mday / 10)).append(std::to_string(timeinfo.tm_mday % 10));
                    break;
                case 'm': // %m - Month (01-12)
                    result.append(std::to_string((timeinfo.tm_mon + 1) / 10)).append(std::to_string((timeinfo.tm_mon + 1) % 10));
                    break;
                case 'Y': // %Y - Year (4 digits)
                    result.append(std::to_string(timeinfo.tm_year + 1900));
                    break;
                case 'l': // %l - Full name of the day of the week
                    if (timeinfo.tm_wday >= 0 && timeinfo.tm_wday < static_cast<int>(defaultConfig.long_day_names.size())) {
                        result.append(defaultConfig.long_day_names[timeinfo.tm_wday]);
                    }
                    break;
                case 'F': // %F - Full name of the month
                    if (timeinfo.tm_mon >= 0 && timeinfo.tm_mon < static_cast<int>(defaultConfig.long_month_names.size())) {
                        result.append(defaultConfig.long_month_names[timeinfo.tm_mon]);
                    }
                    break;
                case 'a': // %a - Abbreviated name of the day of the week
                    if (timeinfo.tm_wday >= 0 && timeinfo.tm_wday < static_cast<int>(defaultConfig.short_day_names.size())) {
                        result.append(defaultConfig.short_day_names[timeinfo.tm_wday]);
                    }
                    break;
                case 'b': // %b - Abbreviated name of the month
                    if (timeinfo.tm_mon >= 0 && timeinfo.tm_mon < static_cast<int>(defaultConfig.short_month_names.size())) {
                        result.append(defaultConfig.short_month_names[timeinfo.tm_mon]);
                    }
                    break;
                default: // Unknown format - keep as is
                    result.push_back('%');
                    result.push_back(formatChar);
                    break;
            }
            
            pos = foundPos + 2; // Skip % and format character
        } else {
            // % at the end of the string
            result.push_back('%');
            pos = foundPos + 1;
        }
    }
    
    return result;
}

std::string I18N::formatNumber(double number) const {
    return formatNumberWithConfig(number, defaultConfig.number);
}

std::string I18N::formatPrice(double amount) const {
    return formatPriceWithConfig(amount, defaultConfig.currency);
}

std::string I18N::formatDate(std::string_view pattern, const std::tm* date) const {
    return formatDateWithConfig(pattern, date, defaultConfig.date_time);
}

const FormatConfig& I18N::getConfig() const noexcept {
    return defaultConfig;
}

void I18N::configure(const json& formats) {
    if (!formats.is_object()) {
        return;
    }
    
    if (formats.contains("currency") && formats["currency"].is_object()) {
        const auto& currency = formats["currency"];
        if (currency.contains("symbol")) defaultConfig.currency.symbol = currency["symbol"].get<std::string>();
        if (currency.contains("name")) defaultConfig.currency.name = currency["name"].get<std::string>();
        if (currency.contains("short_name")) defaultConfig.currency.short_name = currency["short_name"].get<std::string>();
        if (currency.contains("decimal_symbol")) defaultConfig.currency.decimal_symbol = currency["decimal_symbol"].get<std::string>();
        if (currency.contains("thousand_separator")) defaultConfig.currency.thousand_separator = currency["thousand_separator"].get<std::string>();
        if (currency.contains("fract_digits")) defaultConfig.currency.fract_digits = currency["fract_digits"].get<int>();
        if (currency.contains("positive_symbol")) defaultConfig.currency.positive_symbol = currency["positive_symbol"].get<std::string>();
        if (currency.contains("negative_symbol")) defaultConfig.currency.negative_symbol = currency["negative_symbol"].get<std::string>();
        if (currency.contains("positive_format")) defaultConfig.currency.positive_format = currency["positive_format"].get<std::string>();
        if (currency.contains("negative_format")) defaultConfig.currency.negative_format = currency["negative_format"].get<std::string>();
    }
    

    if (formats.contains("number") && formats["number"].is_object()) {
        const auto& number = formats["number"];
        if (number.contains("decimal_symbol")) defaultConfig.number.decimal_symbol = number["decimal_symbol"].get<std::string>();
        if (number.contains("thousand_separator")) defaultConfig.number.thousand_separator = number["thousand_separator"].get<std::string>();
        if (number.contains("fract_digits")) defaultConfig.number.fract_digits = number["fract_digits"].get<int>();
        if (number.contains("positive_symbol")) defaultConfig.number.positive_symbol = number["positive_symbol"].get<std::string>();
        if (number.contains("negative_symbol")) defaultConfig.number.negative_symbol = number["negative_symbol"].get<std::string>();
    }
    

    if (formats.contains("date_time") && formats["date_time"].is_object()) {
        const auto& date_time = formats["date_time"];
        if (date_time.contains("long_time")) defaultConfig.date_time.long_time = date_time["long_time"].get<std::string>();
        if (date_time.contains("short_time")) defaultConfig.date_time.short_time = date_time["short_time"].get<std::string>();
        if (date_time.contains("long_date")) defaultConfig.date_time.long_date = date_time["long_date"].get<std::string>();
        if (date_time.contains("short_date")) defaultConfig.date_time.short_date = date_time["short_date"].get<std::string>();
        if (date_time.contains("long_date_time")) defaultConfig.date_time.long_date_time = date_time["long_date_time"].get<std::string>();
        if (date_time.contains("short_date_time")) defaultConfig.date_time.short_date_time = date_time["short_date_time"].get<std::string>();
    }
    

    if (formats.contains("short_month_names") && formats["short_month_names"].is_array()) {
        defaultConfig.short_month_names.clear();
        defaultConfig.short_month_names.reserve(formats["short_month_names"].size());
        for (const auto& month : formats["short_month_names"]) {
            defaultConfig.short_month_names.push_back(month.get<std::string>());
        }
    }
    
    if (formats.contains("long_month_names") && formats["long_month_names"].is_array()) {
        defaultConfig.long_month_names.clear();
        defaultConfig.long_month_names.reserve(formats["long_month_names"].size());
        for (const auto& month : formats["long_month_names"]) {
            defaultConfig.long_month_names.push_back(month.get<std::string>());
        }
    }
    
    if (formats.contains("short_day_names") && formats["short_day_names"].is_array()) {
        defaultConfig.short_day_names.clear();
        defaultConfig.short_day_names.reserve(formats["short_day_names"].size());
        for (const auto& day : formats["short_day_names"]) {
            defaultConfig.short_day_names.push_back(day.get<std::string>());
        }
    }
    
    if (formats.contains("long_day_names") && formats["long_day_names"].is_array()) {
        defaultConfig.long_day_names.clear();
        defaultConfig.long_day_names.reserve(formats["long_day_names"].size());
        for (const auto& day : formats["long_day_names"]) {
            defaultConfig.long_day_names.push_back(day.get<std::string>());
        }
    }
}

void I18N::reset() {
    locales.clear();
    localesData.clear();
    formatConfigs.clear();
    
    defaultConfig = FormatConfig{};
    
    fallbackLocale = "en";
}

} // namespace i18n
