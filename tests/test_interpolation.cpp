#include <gtest/gtest.h>
#include "i18ncpp.h"
#include <string>

static std::string fixturesDir() {
    return FIXTURES_DIR;
}

class InterpolationTest : public ::testing::Test {
protected:
    i18n::I18N i18n;

    void SetUp() override {
        i18n.loadLocale("en", (fixturesDir() + "/en.json").c_str());
        i18n.setLocale("en");
        i18n.setFallbackLocale("en");
    }
};

// --- Positional interpolation {0}, {1} ---

TEST_F(InterpolationTest, PositionalTwoParams) {
    EXPECT_EQ(i18n.tr("welcome_pos", "Alice", "Wonderland"),
              "Hello Alice, welcome to Wonderland");
}

TEST_F(InterpolationTest, PositionalSingleParam) {
    // {1} has no matching param — should keep placeholder
    EXPECT_EQ(i18n.tr("welcome_pos", {"Alice"}),
              "Hello Alice, welcome to {1}");
}

// --- Unnamed interpolation {} ---

TEST_F(InterpolationTest, UnnamedTwoParams) {
    EXPECT_EQ(i18n.tr("welcome_unnamed", "Alice", "Wonderland"),
              "Hello Alice, welcome to Wonderland");
}

TEST_F(InterpolationTest, UnnamedExhaustedParams) {
    // Only one param for two {} placeholders
    EXPECT_EQ(i18n.tr("welcome_unnamed", {"Alice"}),
              "Hello Alice, welcome to {}");
}

// --- Out-of-range index keeps placeholder ---

TEST_F(InterpolationTest, OutOfRangeIndexKeepsPlaceholder) {
    EXPECT_EQ(i18n.tr("welcome_pos", std::vector<std::string>{}),
              "Hello {0}, welcome to {1}");
}

// --- Empty params returns template ---

TEST_F(InterpolationTest, EmptyParamsReturnsTemplate) {
    EXPECT_EQ(i18n.tr("welcome_pos"), "Hello {0}, welcome to {1}");
}

// --- Named interpolation %{key} ---

TEST_F(InterpolationTest, NamedInterpolation) {
    nlohmann::json params = {{"name", "Alice"}};
    EXPECT_EQ(i18n.translate("welcome_named", params), "Alice has arrived");
}

TEST_F(InterpolationTest, MissingNamedKeyKeepsPlaceholder) {
    nlohmann::json params = {{"other", "value"}};
    EXPECT_EQ(i18n.translate("welcome_named", params), "%{name} has arrived");
}

// --- Formatted interpolation %<key>.fmt ---

TEST_F(InterpolationTest, FormattedIntegerD) {
    nlohmann::json params = {{"name", "Alice"}, {"score", 42.7}};
    EXPECT_EQ(i18n.translate("score_fmt", params), "Alice scored 42 points");
}

TEST_F(InterpolationTest, FormattedFloatF) {
    nlohmann::json params = {{"amount", 3.14}};
    EXPECT_EQ(i18n.translate("price_fmt", params), "Total: 3.14");
}

// --- No placeholders returns string unchanged ---

TEST_F(InterpolationTest, NoPlaceholdersUnchanged) {
    EXPECT_EQ(i18n.tr("no_placeholders"), "Plain text");
}

TEST_F(InterpolationTest, NoPlaceholdersWithParams) {
    EXPECT_EQ(i18n.tr("no_placeholders", "ignored"), "Plain text");
}
