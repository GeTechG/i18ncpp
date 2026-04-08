#include <gtest/gtest.h>
#include "i18ncpp.h"
#include <string>
#include <ctime>

static std::string fixturesDir() {
    return FIXTURES_DIR;
}

class FormattingTest : public ::testing::Test {
protected:
    i18n::I18N i18n;

    void SetUp() override {
        i18n.loadLocale("en", (fixturesDir() + "/en.json").c_str());
        i18n.setLocale("en");
    }

    // Helper: create a deterministic tm for 2026-03-15 14:30:45 (Saturday)
    std::tm makeFixedTm() {
        std::tm tm = {};
        tm.tm_year = 126;   // 2026 - 1900
        tm.tm_mon = 2;      // March (0-based)
        tm.tm_mday = 15;
        tm.tm_hour = 14;
        tm.tm_min = 30;
        tm.tm_sec = 45;
        tm.tm_wday = 6;     // Saturday
        return tm;
    }
};

// --- Number Formatting ---

TEST_F(FormattingTest, FormatNumberDefault) {
    // Default config: space thousand separator, dot decimal, 2 fract digits
    EXPECT_EQ(i18n.formatNumber(1234567.89), "1 234 567.89");
}

TEST_F(FormattingTest, FormatNumberSmallDecimal) {
    EXPECT_EQ(i18n.formatNumber(0.5), "0.50");
}

TEST_F(FormattingTest, FormatNumberNegative) {
    EXPECT_EQ(i18n.formatNumber(-42.5), "-42.50");
}

TEST_F(FormattingTest, FormatNumberCustomConfig) {
    // Custom config: comma thousand separator, 3 fractional digits
    i18n.configure(nlohmann::json::parse(R"({
        "number": {
            "thousand_separator": ",",
            "fract_digits": 3
        }
    })"));
    EXPECT_EQ(i18n.formatNumber(1234.5), "1,234.500");
}

// --- Currency Formatting ---

TEST_F(FormattingTest, FormatPricePositive) {
    i18n.configure(nlohmann::json::parse(R"({
        "currency": {
            "symbol": "$",
            "positive_format": "%c%p%q",
            "negative_format": "%c%p%q",
            "decimal_symbol": ".",
            "thousand_separator": ","
        }
    })"));
    EXPECT_EQ(i18n.formatPrice(1234.56), "$1,234.56");
}

TEST_F(FormattingTest, FormatPriceNegative) {
    i18n.configure(nlohmann::json::parse(R"({
        "currency": {
            "symbol": "$",
            "positive_format": "%c%p%q",
            "negative_format": "%c%p%q",
            "decimal_symbol": ".",
            "thousand_separator": ","
        }
    })"));
    EXPECT_EQ(i18n.formatPrice(-9.99), "$-9.99");
}

// --- Date Formatting ---

TEST_F(FormattingTest, FormatDateShortDate) {
    std::tm tm = makeFixedTm();
    EXPECT_EQ(i18n.formatDate("short_date", &tm), "03/15/2026");
}

TEST_F(FormattingTest, FormatDateLongDate) {
    // Configure long_date to use %F (supported full month name specifier)
    i18n.configure(nlohmann::json::parse(R"({
        "date_time": {
            "long_date": "%F %d, %Y"
        }
    })"));
    std::tm tm = makeFixedTm();
    EXPECT_EQ(i18n.formatDate("long_date", &tm), "March 15, 2026");
}

TEST_F(FormattingTest, FormatDateShortTime) {
    std::tm tm = makeFixedTm();
    EXPECT_EQ(i18n.formatDate("short_time", &tm), "14:30");
}

TEST_F(FormattingTest, FormatDateLongTime) {
    std::tm tm = makeFixedTm();
    EXPECT_EQ(i18n.formatDate("long_time", &tm), "14:30:45");
}

TEST_F(FormattingTest, FormatDateCustomPattern) {
    std::tm tm = makeFixedTm();
    EXPECT_EQ(i18n.formatDate("%Y-%m-%d", &tm), "2026-03-15");
}

TEST_F(FormattingTest, FormatDate12Hour) {
    std::tm tm = makeFixedTm();
    EXPECT_EQ(i18n.formatDate("%I:%M %p", &tm), "02:30 PM");
}

TEST_F(FormattingTest, FormatDateAbbreviatedNames) {
    std::tm tm = makeFixedTm();
    EXPECT_EQ(i18n.formatDate("%a %b %d", &tm), "Sat Mar 15");
}

TEST_F(FormattingTest, FormatDateFullNames) {
    std::tm tm = makeFixedTm();
    EXPECT_EQ(i18n.formatDate("%l, %F %d", &tm), "Saturday, March 15");
}

TEST_F(FormattingTest, FormatDateEmptyPatternISO) {
    std::tm tm = makeFixedTm();
    EXPECT_EQ(i18n.formatDate("", &tm), "2026-03-15T14:30:45");
}

// --- Configure ---

TEST_F(FormattingTest, ConfigureAppliesSettings) {
    i18n.configure(nlohmann::json::parse(R"({
        "number": {
            "decimal_symbol": ",",
            "thousand_separator": ".",
            "fract_digits": 1
        }
    })"));

    const auto& config = i18n.getConfig();
    EXPECT_EQ(config.number.decimal_symbol, ",");
    EXPECT_EQ(config.number.thousand_separator, ".");
    EXPECT_EQ(config.number.fract_digits, 1);

    EXPECT_EQ(i18n.formatNumber(1234.5), "1.234,5");
}
