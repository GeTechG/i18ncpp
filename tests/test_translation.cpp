#include <gtest/gtest.h>
#include "i18ncpp.h"
#include <string>
#include <vector>

static std::string fixturesDir() {
    return FIXTURES_DIR;
}

class TranslationTest : public ::testing::Test {
protected:
    i18n::I18N i18n;

    void SetUp() override {
        i18n.loadLocale("en", (fixturesDir() + "/en.json").c_str());
        i18n.loadLocale("fr", (fixturesDir() + "/fr.json").c_str());
        i18n.loadLocale("ru", (fixturesDir() + "/ru.json").c_str());
        i18n.setLocale("en");
        i18n.setFallbackLocale("en");
    }
};

// --- Basic Translation ---

TEST_F(TranslationTest, BasicTranslation) {
    EXPECT_EQ(i18n.tr("greeting"), "Hello");
    EXPECT_EQ(i18n.tr("farewell"), "Goodbye");
}

TEST_F(TranslationTest, BasicTranslationFrench) {
    i18n.setLocale("fr");
    EXPECT_EQ(i18n.tr("greeting"), "Bonjour");
    EXPECT_EQ(i18n.tr("farewell"), "Au revoir");
}

TEST_F(TranslationTest, BasicTranslationRussian) {
    i18n.setLocale("ru");
    EXPECT_EQ(i18n.tr("greeting"), "Привет");
}

// --- Nested Keys ---

TEST_F(TranslationTest, NestedKeys) {
    EXPECT_EQ(i18n.tr("menu.file.open"), "Open");
    EXPECT_EQ(i18n.tr("menu.file.save"), "Save");
    EXPECT_EQ(i18n.tr("menu.file.close"), "Close");
    EXPECT_EQ(i18n.tr("menu.edit.copy"), "Copy");
    EXPECT_EQ(i18n.tr("menu.edit.paste"), "Paste");
}

TEST_F(TranslationTest, DeepNestedKeys) {
    EXPECT_EQ(i18n.tr("deep.nested.key"), "Deep value");
}

TEST_F(TranslationTest, NestedKeysFrench) {
    i18n.setLocale("fr");
    EXPECT_EQ(i18n.tr("menu.file.open"), "Ouvrir");
    EXPECT_EQ(i18n.tr("menu.file.save"), "Enregistrer");
}

// --- Array Indexing ---

TEST_F(TranslationTest, ArrayIndexing) {
    EXPECT_EQ(i18n.tr("items.0"), "Apple");
    EXPECT_EQ(i18n.tr("items.1"), "Banana");
    EXPECT_EQ(i18n.tr("items.2"), "Cherry");
}

TEST_F(TranslationTest, ArrayIndexOutOfBounds) {
    EXPECT_EQ(i18n.tr("items.99"), "items.99");
}

// --- Fallback Locale ---

TEST_F(TranslationTest, FallbackToEnglish) {
    i18n.setLocale("fr");
    // "only_in_en" exists in en.json but not fr.json
    EXPECT_EQ(i18n.tr("only_in_en"), "English only");
}

TEST_F(TranslationTest, FallbackNestedKey) {
    i18n.setLocale("fr");
    // "menu.file.close" exists in en but not fr
    EXPECT_EQ(i18n.tr("menu.file.close"), "Close");
}

TEST_F(TranslationTest, FallbackDeepNested) {
    i18n.setLocale("fr");
    EXPECT_EQ(i18n.tr("deep.nested.key"), "Deep value");
}

// --- Locale Ancestry ---

TEST_F(TranslationTest, LocaleAncestryFallback) {
    // "en-US" should fall back to "en"
    i18n.setLocale("en-US");
    EXPECT_EQ(i18n.tr("greeting"), "Hello");
}

TEST_F(TranslationTest, LocaleAncestryDeepFallback) {
    i18n.setLocale("en-US");
    EXPECT_EQ(i18n.tr("menu.file.open"), "Open");
}

// --- keyExists ---

TEST_F(TranslationTest, KeyExistsPresent) {
    EXPECT_TRUE(i18n.keyExists("greeting"));
    EXPECT_TRUE(i18n.keyExists("menu.file.open"));
    EXPECT_TRUE(i18n.keyExists("deep.nested.key"));
}

TEST_F(TranslationTest, KeyExistsAbsent) {
    EXPECT_FALSE(i18n.keyExists("nonexistent"));
    EXPECT_FALSE(i18n.keyExists("menu.file.nonexistent"));
    EXPECT_FALSE(i18n.keyExists("totally.made.up.key"));
}

// --- Missing Key ---

TEST_F(TranslationTest, MissingKeyReturnsKeyString) {
    EXPECT_EQ(i18n.tr("nonexistent.key"), "nonexistent.key");
}

TEST_F(TranslationTest, MissingNestedKeyReturnsKeyString) {
    EXPECT_EQ(i18n.tr("menu.file.nonexistent"), "menu.file.nonexistent");
}

// --- Empty Key ---

TEST_F(TranslationTest, EmptyKeyReturnsEmpty) {
    EXPECT_EQ(i18n.tr(""), "");
}

// --- Locale Management ---

TEST_F(TranslationTest, SetLocaleAndGetLocale) {
    i18n.setLocale("fr");
    EXPECT_EQ(i18n.getLocale(), "fr");
}

TEST_F(TranslationTest, SetMultipleLocales) {
    std::vector<std::string> locales = {"fr", "en"};
    i18n.setLocale(locales);
    EXPECT_EQ(i18n.getLocales().size(), 2u);
    EXPECT_EQ(i18n.getLocales()[0], "fr");
    EXPECT_EQ(i18n.getLocales()[1], "en");
}

TEST_F(TranslationTest, SetAndGetFallbackLocale) {
    i18n.setFallbackLocale("de");
    EXPECT_EQ(i18n.getFallbackLocale(), "de");
}

TEST_F(TranslationTest, ResetClearsState) {
    i18n.reset();
    EXPECT_EQ(i18n.getLocale(), "");
    EXPECT_EQ(i18n.getFallbackLocale(), "en");
    EXPECT_TRUE(i18n.getLocales().empty());
}

// --- loadLocaleFromFile ---

TEST_F(TranslationTest, LoadLocaleFromFileExtractsName) {
    i18n::I18N fresh;
    fresh.loadLocaleFromFile((fixturesDir() + "/en.json").c_str());
    fresh.setLocale("en");
    EXPECT_EQ(fresh.tr("greeting"), "Hello");
}
