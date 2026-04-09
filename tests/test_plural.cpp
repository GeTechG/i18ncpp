#include <gtest/gtest.h>
#include "i18ncpp.h"
#include <string>

static std::string fixturesDir() {
    return FIXTURES_DIR;
}

class PluralTest : public ::testing::Test {
protected:
    i18n::I18N i18n;

    void SetUp() override {
        i18n.loadLocale("en", (fixturesDir() + "/en.json").c_str());
        i18n.loadLocale("fr", (fixturesDir() + "/fr.json").c_str());
        i18n.loadLocale("ru", (fixturesDir() + "/ru.json").c_str());
        i18n.loadLocale("pl", (fixturesDir() + "/pl.json").c_str());
        i18n.loadLocale("ar", (fixturesDir() + "/ar.json").c_str());
        i18n.loadLocale("cs", (fixturesDir() + "/cs.json").c_str());
        i18n.setLocale("en");
        i18n.setFallbackLocale("en");
    }
};

// --- English plural rules (one/other) ---

TEST_F(PluralTest, EnglishOne) {
    i18n.setLocale("en");
    EXPECT_EQ(i18n.trPlural("items_plural", 1), "1 item");
}

TEST_F(PluralTest, EnglishOther) {
    i18n.setLocale("en");
    EXPECT_EQ(i18n.trPlural("items_plural", 0), "0 items");
    EXPECT_EQ(i18n.trPlural("items_plural", 2), "2 items");
    EXPECT_EQ(i18n.trPlural("items_plural", 5), "5 items");
}

// --- Russian plural rules (one/few/many/other) ---

TEST_F(PluralTest, RussianOne) {
    i18n.setLocale("ru");
    EXPECT_EQ(i18n.trPlural("items_plural", 1), "1 предмет");
    EXPECT_EQ(i18n.trPlural("items_plural", 21), "21 предмет");
    EXPECT_EQ(i18n.trPlural("items_plural", 101), "101 предмет");
}

TEST_F(PluralTest, RussianFew) {
    i18n.setLocale("ru");
    EXPECT_EQ(i18n.trPlural("items_plural", 2), "2 предмета");
    EXPECT_EQ(i18n.trPlural("items_plural", 3), "3 предмета");
    EXPECT_EQ(i18n.trPlural("items_plural", 4), "4 предмета");
    EXPECT_EQ(i18n.trPlural("items_plural", 22), "22 предмета");
}

TEST_F(PluralTest, RussianMany) {
    i18n.setLocale("ru");
    EXPECT_EQ(i18n.trPlural("items_plural", 5), "5 предметов");
    EXPECT_EQ(i18n.trPlural("items_plural", 11), "11 предметов");
    EXPECT_EQ(i18n.trPlural("items_plural", 111), "111 предметов");
}

// --- French plural rules (count < 2 = one) ---

TEST_F(PluralTest, FrenchOne) {
    i18n.setLocale("fr");
    EXPECT_EQ(i18n.trPlural("items_plural", 0), "0 élément");
    EXPECT_EQ(i18n.trPlural("items_plural", 1), "1 élément");
}

TEST_F(PluralTest, FrenchOther) {
    i18n.setLocale("fr");
    EXPECT_EQ(i18n.trPlural("items_plural", 2), "2 éléments");
    EXPECT_EQ(i18n.trPlural("items_plural", 5), "5 éléments");
}

// --- Polish plural rules (one/few/many) ---

TEST_F(PluralTest, PolishOne) {
    i18n.setLocale("pl");
    EXPECT_EQ(i18n.trPlural("items_plural", 1), "1 przedmiot");
}

TEST_F(PluralTest, PolishFew) {
    i18n.setLocale("pl");
    EXPECT_EQ(i18n.trPlural("items_plural", 2), "2 przedmioty");
    EXPECT_EQ(i18n.trPlural("items_plural", 3), "3 przedmioty");
    EXPECT_EQ(i18n.trPlural("items_plural", 4), "4 przedmioty");
    EXPECT_EQ(i18n.trPlural("items_plural", 22), "22 przedmioty");
}

TEST_F(PluralTest, PolishMany) {
    i18n.setLocale("pl");
    EXPECT_EQ(i18n.trPlural("items_plural", 5), "5 przedmiotów");
    EXPECT_EQ(i18n.trPlural("items_plural", 12), "12 przedmiotów");
    EXPECT_EQ(i18n.trPlural("items_plural", 0), "0 przedmiotów");
}

// --- Czech plural rules (one/few/other) ---

TEST_F(PluralTest, CzechOne) {
    i18n.setLocale("cs");
    EXPECT_EQ(i18n.trPlural("items_plural", 1), "1 položka");
}

TEST_F(PluralTest, CzechFew) {
    i18n.setLocale("cs");
    EXPECT_EQ(i18n.trPlural("items_plural", 2), "2 položky");
    EXPECT_EQ(i18n.trPlural("items_plural", 3), "3 položky");
    EXPECT_EQ(i18n.trPlural("items_plural", 4), "4 položky");
}

TEST_F(PluralTest, CzechOther) {
    i18n.setLocale("cs");
    EXPECT_EQ(i18n.trPlural("items_plural", 5), "5 položek");
    EXPECT_EQ(i18n.trPlural("items_plural", 0), "0 položek");
}

// --- Arabic plural rules (zero/one/two/few/many/other) ---

TEST_F(PluralTest, ArabicZero) {
    i18n.setLocale("ar");
    EXPECT_EQ(i18n.trPlural("items_plural", 0), "0 عنصر");
}

TEST_F(PluralTest, ArabicOne) {
    i18n.setLocale("ar");
    EXPECT_EQ(i18n.trPlural("items_plural", 1), "1 عنصر");
}

TEST_F(PluralTest, ArabicTwo) {
    i18n.setLocale("ar");
    EXPECT_EQ(i18n.trPlural("items_plural", 2), "2 عنصران");
}

TEST_F(PluralTest, ArabicFew) {
    i18n.setLocale("ar");
    EXPECT_EQ(i18n.trPlural("items_plural", 3), "3 عناصر");
    EXPECT_EQ(i18n.trPlural("items_plural", 10), "10 عناصر");
}

TEST_F(PluralTest, ArabicMany) {
    i18n.setLocale("ar");
    EXPECT_EQ(i18n.trPlural("items_plural", 11), "11 عنصرًا");
    EXPECT_EQ(i18n.trPlural("items_plural", 99), "99 عنصرًا");
}

TEST_F(PluralTest, ArabicOther) {
    i18n.setLocale("ar");
    EXPECT_EQ(i18n.trPlural("items_plural", 100), "100 عنصر");
}

// --- Pluralization with extra params ---

TEST_F(PluralTest, PluralWithExtraParams) {
    i18n.setLocale("en");
    EXPECT_EQ(i18n.trPlural("items_in_cart", 3, {"cart"}), "3 items in cart");
    EXPECT_EQ(i18n.trPlural("items_in_cart", 1, {"bag"}), "1 item in bag");
}

// --- Fallback to "other" when specific form missing ---

TEST_F(PluralTest, FallbackToOther) {
    i18n.setLocale("en");
    // English only has "one" and "other", count=0 should use "other"
    EXPECT_EQ(i18n.trPlural("items_plural", 0), "0 items");
}
