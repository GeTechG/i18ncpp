#include <gtest/gtest.h>
#include "i18ncpp.h"
#include <string>
#include <ctime>

static std::string fixturesDir() {
    return FIXTURES_DIR;
}

class FormatCacheTest : public ::testing::Test {
protected:
    i18n::I18N i18n;

    void SetUp() override {
        i18n.loadLocale("en", (fixturesDir() + "/en.json").c_str());
        i18n.setLocale("en");
    }

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

TEST_F(FormatCacheTest, CacheHitReturnsSameResult) {
    std::string first = i18n.formatNumber(1234.56);
    EXPECT_GT(i18n.formatCacheSize(), 0u);
    std::string second = i18n.formatNumber(1234.56);
    EXPECT_EQ(first, second);
}

TEST_F(FormatCacheTest, CacheHitFormatPrice) {
    std::string first = i18n.formatPrice(99.99);
    EXPECT_GT(i18n.formatCacheSize(), 0u);
    std::string second = i18n.formatPrice(99.99);
    EXPECT_EQ(first, second);
}

TEST_F(FormatCacheTest, CacheHitFormatDate) {
    std::tm tm = makeFixedTm();
    std::string first = i18n.formatDate("short_date", &tm);
    EXPECT_GT(i18n.formatCacheSize(), 0u);
    std::string second = i18n.formatDate("short_date", &tm);
    EXPECT_EQ(first, second);
}

TEST_F(FormatCacheTest, InvalidateOnSetLocale) {
    i18n.formatNumber(1234.56);
    EXPECT_GT(i18n.formatCacheSize(), 0u);
    i18n.setLocale("en");
    EXPECT_EQ(i18n.formatCacheSize(), 0u);
}

TEST_F(FormatCacheTest, InvalidateOnConfigure) {
    i18n.formatPrice(99.99);
    EXPECT_GT(i18n.formatCacheSize(), 0u);

    nlohmann::json newConfig = {
        {"currency", {{"symbol", "EUR"}, {"decimal_symbol", ","}, {"thousand_separator", "."}}}
    };
    i18n.configure(newConfig);
    EXPECT_EQ(i18n.formatCacheSize(), 0u);

    // Next call should use new config
    std::string result = i18n.formatPrice(99.99);
    EXPECT_NE(result.find("EUR"), std::string::npos);
}

TEST_F(FormatCacheTest, InvalidateOnReset) {
    i18n.formatNumber(42.0);
    EXPECT_GT(i18n.formatCacheSize(), 0u);
    i18n.reset();
    EXPECT_EQ(i18n.formatCacheSize(), 0u);
}

TEST_F(FormatCacheTest, NoCacheForNullptrDate) {
    // formatDate with nullptr uses current time -- should not cache
    i18n.formatDate("short_time", nullptr);
    // Cache should not grow for time-dependent calls
    size_t sizeAfter = i18n.formatCacheSize();
    EXPECT_EQ(sizeAfter, 0u);
}
