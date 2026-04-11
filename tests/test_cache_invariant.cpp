#include <gtest/gtest.h>
#include "i18ncpp.h"

#include <functional>
#include <string>
#include <vector>

static std::string fixturesDir() {
    return FIXTURES_DIR;
}

struct MutatorCase {
    std::string name;
    std::function<void(i18n::I18N&)> mutate;
};

class CacheInvariantTest : public ::testing::TestWithParam<MutatorCase> {
protected:
    i18n::I18N i18n;

    void SetUp() override {
        i18n.loadLocale("en", (fixturesDir() + "/en.json").c_str());
        i18n.loadLocale("fr", (fixturesDir() + "/fr.json").c_str());
        i18n.setLocale("en");
    }
};

TEST_P(CacheInvariantTest, ClearsBothCachesAfterMutation) {
    (void)i18n.tr("greeting");
    (void)i18n.formatNumber(1234.56);

    ASSERT_GT(i18n.translationCacheSize(), 0u)
        << "translation cache not warmed before mutator " << GetParam().name;
    ASSERT_GT(i18n.formatCacheSize(), 0u)
        << "format cache not warmed before mutator " << GetParam().name;

    GetParam().mutate(i18n);

    EXPECT_EQ(i18n.formatCacheSize(), 0u)
        << "format cache not cleared after " << GetParam().name;
    EXPECT_EQ(i18n.translationCacheSize(), 0u)
        << "translation cache not cleared after " << GetParam().name;
}

INSTANTIATE_TEST_SUITE_P(
    AllMutators,
    CacheInvariantTest,
    ::testing::Values(
        MutatorCase{"loadLocale",
                    [](i18n::I18N& i) {
                        i.loadLocale("cs", (fixturesDir() + "/cs.json").c_str());
                    }},
        MutatorCase{"mergeLocale",
                    [](i18n::I18N& i) {
                        i.mergeLocale("en", (fixturesDir() + "/merge_extra.json").c_str());
                    }},
        MutatorCase{"load_json",
                    [](i18n::I18N& i) {
                        i18n::json data = {{"de", {{"greeting", "Hallo"}}}};
                        i.load(data);
                    }},
        MutatorCase{"setLocale_sv",
                    [](i18n::I18N& i) {
                        i.setLocale("fr");
                    }},
        MutatorCase{"setLocale_vector",
                    [](i18n::I18N& i) {
                        i.setLocale(std::vector<std::string>{"fr", "en"});
                    }},
        MutatorCase{"setFallbackLocale",
                    [](i18n::I18N& i) {
                        i.setFallbackLocale("fr");
                    }},
        MutatorCase{"configure",
                    [](i18n::I18N& i) {
                        i18n::json fmt = {{"currency", {{"symbol", "EUR"}}}};
                        i.configure(fmt);
                    }},
        MutatorCase{"reset",
                    [](i18n::I18N& i) {
                        i.reset();
                    }}
    ),
    [](const ::testing::TestParamInfo<MutatorCase>& info) {
        return info.param.name;
    }
);
