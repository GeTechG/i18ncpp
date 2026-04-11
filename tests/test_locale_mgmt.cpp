#include <gtest/gtest.h>
#include "i18ncpp.h"
#include <fstream>
#include <string>
#include <string_view>

static std::string fixturesDir() {
    return FIXTURES_DIR;
}

class LocaleMgmtTest : public ::testing::Test {
protected:
    i18n::I18N i18n;
};

// --- mergeLocale ---

TEST_F(LocaleMgmtTest, MergeLocaleAddsNewKeys) {
    i18n.loadLocale("en", (fixturesDir() + "/en.json").c_str());
    i18n.setLocale("en");

    // Original greeting
    EXPECT_EQ(i18n.tr("greeting"), "Hello");

    // Merge extra translations
    i18n.mergeLocale("en", (fixturesDir() + "/merge_extra.json").c_str());

    // New key accessible
    EXPECT_EQ(i18n.tr("extra_key"), "Extra value");

    // Merged key overrides original
    EXPECT_EQ(i18n.tr("greeting"), "Hello override");

    // Original keys still work
    EXPECT_EQ(i18n.tr("farewell"), "Goodbye");
    EXPECT_EQ(i18n.tr("menu.file.open"), "Open");
}

// --- mergeLocaleFromFile ---

TEST_F(LocaleMgmtTest, MergeLocaleFromFileExtractsLocale) {
    // mergeLocaleFromFile extracts locale "en" from filename
    i18n.mergeLocaleFromFile((fixturesDir() + "/en.json").c_str());
    i18n.setLocale("en");

    EXPECT_EQ(i18n.tr("greeting"), "Hello");
    EXPECT_EQ(i18n.tr("farewell"), "Goodbye");
}

// --- reset ---

TEST_F(LocaleMgmtTest, ResetClearsAllState) {
    // Set up state: load locale, configure format, set locale and fallback
    i18n.loadLocale("en", (fixturesDir() + "/en.json").c_str());
    i18n.setLocale("en");
    i18n.setFallbackLocale("en");
    i18n.configure(nlohmann::json::parse(R"({
        "number": {
            "decimal_symbol": ",",
            "thousand_separator": ".",
            "fract_digits": 4
        }
    })"));

    // Verify state is set
    EXPECT_EQ(i18n.getLocale(), "en");
    EXPECT_EQ(i18n.tr("greeting"), "Hello");
    EXPECT_EQ(i18n.getConfig().number.fract_digits, 4);

    // Reset
    i18n.reset();

    // Locale cleared (getLocale returns "" when locales vector is empty)
    EXPECT_EQ(i18n.getLocale(), "");

    // Config back to defaults
    EXPECT_EQ(i18n.getConfig().number.decimal_symbol, ".");
    EXPECT_EQ(i18n.getConfig().number.thousand_separator, " ");
    EXPECT_EQ(i18n.getConfig().number.fract_digits, 2);

    // Translations gone — tr returns the key itself
    EXPECT_EQ(i18n.tr("greeting"), "greeting");
}

// --- AC-1: configure() strong exception safety ---

TEST_F(LocaleMgmtTest, ConfigureRollbackOnTypeError) {
    i18n.loadLocale("en", (fixturesDir() + "/en.json").c_str());
    i18n.setLocale("en");

    // Prime the format cache
    (void)i18n.formatPrice(123.45);
    const size_t cachePre = i18n.formatCacheSize();
    EXPECT_GT(cachePre, 0u);

    const std::string prevSymbol = i18n.getConfig().currency.symbol;

    // Load hostile fixture: _formats.currency.symbol is an array → json::type_error mid-configure
    nlohmann::json hostile;
    std::ifstream hostileStream(fixturesDir() + "/formats_wrong_type.json");
    ASSERT_TRUE(hostileStream.is_open());
    hostileStream >> hostile;

    EXPECT_THROW(i18n.configure(hostile["_formats"]), ::i18n::I18NError);

    // Rollback: defaultConfig unchanged
    EXPECT_EQ(i18n.getConfig().currency.symbol, prevSymbol);
    // Commit window never reached → cache unchanged and still valid against unchanged source
    EXPECT_EQ(i18n.formatCacheSize(), cachePre);
}

// --- AC-2: loadLocale translates json::type_error to I18NError ---

TEST_F(LocaleMgmtTest, LoadLocaleCatchesJsonTypeError) {
    // Hostile fixture parses cleanly but type-errors inside configure() via loadLocale.
    const std::string path = fixturesDir() + "/formats_wrong_type.json";
    EXPECT_THROW(i18n.loadLocale("en", path), ::i18n::I18NError);
}

// --- AC-3: loadLocale accepts non-null-terminated string_view ---

TEST_F(LocaleMgmtTest, LoadLocaleAcceptsNonNullTerminatedView) {
    // Backing buffer: real path followed by garbage, no null in the view's range.
    const std::string realPath = fixturesDir() + "/en.json";
    std::string backing = realPath + "XXXXXXXX";
    std::string_view sv(backing.data(), realPath.size());

    EXPECT_NO_THROW(i18n.loadLocale("en", sv));
    i18n.setLocale("en");
    EXPECT_EQ(i18n.tr("greeting"), "Hello");
}
