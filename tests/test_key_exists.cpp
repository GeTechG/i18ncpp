#include <gtest/gtest.h>
#include <i18ncpp.h>

using namespace i18n;

TEST(KeyExists, ExactKeyFound) {
    I18N i;
    json data = {
        {"en", {
            {"greeting", "hello"}
        }}
    };
    i.load(data);
    i.setLocale("en");

    EXPECT_TRUE(i.keyExists("greeting"));
    EXPECT_FALSE(i.keyExists("nonexistent"));
}

TEST(KeyExists, PluralBaseKeyResolvesViaOtherSuffix) {
    I18N i;
    json data = {
        {"en", {
            {"items", {
                {"one", "{1} item"},
                {"other", "{1} items"}
            }}
        }}
    };
    i.load(data);
    i.setLocale("en");

    // "items" itself is not a flat key, but "items.other" exists
    // keyExists should return true, matching tr() resolution
    EXPECT_TRUE(i.keyExists("items"));
    EXPECT_TRUE(i.keyExists("items.one"));
    EXPECT_TRUE(i.keyExists("items.other"));
}

TEST(KeyExists, FallbackLocaleResolution) {
    I18N i;
    json data = {
        {"en", {
            {"greeting", "hello"}
        }}
    };
    i.load(data);
    i.setLocale("fr");
    i.setFallbackLocale("en");

    // "greeting" only exists in "en" (fallback), but keyExists should find it
    EXPECT_TRUE(i.keyExists("greeting"));
}
