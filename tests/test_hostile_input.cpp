#include <gtest/gtest.h>
#include "i18ncpp.h"

#include <string>

static std::string fixturesDir() {
    return FIXTURES_DIR;
}

TEST(HostileInput, MissingFileThrowsI18NError) {
    i18n::I18N inst;
    EXPECT_THROW(
        inst.loadLocale("en", fixturesDir() + "/hostile/does_not_exist.json"),
        i18n::I18NError
    );
}

TEST(HostileInput, MalformedJsonThrowsI18NError) {
    i18n::I18N inst;
    EXPECT_THROW(
        inst.loadLocale("en", fixturesDir() + "/hostile/malformed_json.json"),
        i18n::I18NError
    );
}

TEST(HostileInput, TopLevelArrayThrowsI18NError) {
    i18n::I18N inst;
    EXPECT_THROW(
        inst.loadLocale("en", fixturesDir() + "/hostile/not_an_object.json"),
        i18n::I18NError
    );
}

TEST(HostileInput, LeafIsIntSilentlySkipped) {
    i18n::I18N inst;
    EXPECT_NO_THROW(
        inst.loadLocale("en", fixturesDir() + "/hostile/leaf_is_int.json")
    );
    inst.setLocale("en");
    EXPECT_EQ(inst.tr("greeting"), "greeting");
    EXPECT_EQ(inst.tr("farewell"), "bye");
}

TEST(HostileInput, FormatsWrongTypeThrowsI18NErrorAndPreservesCacheInvariant) {
    i18n::I18N inst;
    inst.loadLocale("en", fixturesDir() + "/en.json");
    inst.setLocale("en");

    (void)inst.tr("greeting");
    (void)inst.formatNumber(1234.56);
    const size_t translationCachePre = inst.translationCacheSize();
    const size_t formatCachePre = inst.formatCacheSize();
    ASSERT_GT(translationCachePre, 0u);
    ASSERT_GT(formatCachePre, 0u);

    EXPECT_THROW(
        inst.loadLocale("en", fixturesDir() + "/formats_wrong_type.json"),
        i18n::I18NError
    );

    EXPECT_EQ(inst.formatCacheSize(), formatCachePre);
    EXPECT_EQ(inst.translationCacheSize(), translationCachePre);
}
