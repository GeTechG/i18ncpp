#include <gtest/gtest.h>
#include <i18ncpp.h>

using namespace i18n;

TEST(LoadReplace, StaleKeysRemovedAfterReload) {
    I18N i;
    json data1 = {
        {"en", {
            {"greeting", "hello"},
            {"farewell", "bye"}
        }}
    };
    i.load(data1);
    i.setLocale("en");

    EXPECT_EQ(i.tr("greeting"), "hello");
    EXPECT_EQ(i.tr("farewell"), "bye");

    // Reload with only "greeting" — "farewell" must disappear
    json data2 = {
        {"en", {
            {"greeting", "hi"}
        }}
    };
    i.load(data2);

    EXPECT_EQ(i.tr("greeting"), "hi");
    // Stale key must be gone — tr returns the key itself when not found
    EXPECT_EQ(i.tr("farewell"), "farewell");
}

TEST(LoadReplace, OtherLocalesUnaffected) {
    I18N i;
    json data1 = {
        {"en", {{"greeting", "hello"}}},
        {"fr", {{"greeting", "bonjour"}}}
    };
    i.load(data1);

    // Reload only "en" — "fr" must survive
    json data2 = {
        {"en", {{"greeting", "hi"}}}
    };
    i.load(data2);

    i.setLocale("en");
    EXPECT_EQ(i.tr("greeting"), "hi");

    i.setLocale("fr");
    EXPECT_EQ(i.tr("greeting"), "bonjour");
}
