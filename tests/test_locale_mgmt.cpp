#include <gtest/gtest.h>
#include "i18ncpp.h"
#include <string>

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
