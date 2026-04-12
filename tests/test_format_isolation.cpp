#include <gtest/gtest.h>
#include <i18ncpp.h>

using namespace i18n;

TEST(FormatIsolation, SwitchToLocaleWithoutFormatsResetsToBaseline) {
    I18N i;

    // Load "de" with custom decimal_symbol via _formats
    json data = {
        {"de", {
            {"_formats", {
                {"number", {
                    {"decimal_symbol", ","},
                    {"thousand_separator", "."},
                    {"fract_digits", 2},
                    {"positive_symbol", ""},
                    {"negative_symbol", "-"}
                }}
            }},
            {"greeting", "Hallo"}
        }},
        {"en", {
            {"greeting", "Hello"}
        }}
    };
    i.load(data);

    // Switch to "de" — should pick up comma decimal
    i.setLocale("de");
    EXPECT_EQ(i.getConfig().number.decimal_symbol, ",");

    // Switch to "en" — no _formats → must reset to baseline default "."
    i.setLocale("en");
    EXPECT_EQ(i.getConfig().number.decimal_symbol, ".");
}

TEST(FormatIsolation, SwitchBackToFormattedLocaleRestoresConfig) {
    I18N i;

    json data = {
        {"de", {
            {"_formats", {
                {"number", {
                    {"decimal_symbol", ","},
                    {"thousand_separator", "."},
                    {"fract_digits", 2},
                    {"positive_symbol", ""},
                    {"negative_symbol", "-"}
                }}
            }},
            {"greeting", "Hallo"}
        }},
        {"en", {
            {"greeting", "Hello"}
        }}
    };
    i.load(data);

    i.setLocale("de");
    EXPECT_EQ(i.getConfig().number.decimal_symbol, ",");

    i.setLocale("en");
    EXPECT_EQ(i.getConfig().number.decimal_symbol, ".");

    // Switch back to "de" — must restore comma
    i.setLocale("de");
    EXPECT_EQ(i.getConfig().number.decimal_symbol, ",");
}

TEST(FormatIsolation, VectorSetLocaleAlsoResetsToBaseline) {
    I18N i;

    json data = {
        {"de", {
            {"_formats", {
                {"number", {
                    {"decimal_symbol", ","},
                    {"thousand_separator", "."},
                    {"fract_digits", 2},
                    {"positive_symbol", ""},
                    {"negative_symbol", "-"}
                }}
            }},
            {"greeting", "Hallo"}
        }},
        {"en", {
            {"greeting", "Hello"}
        }}
    };
    i.load(data);

    i.setLocale("de");
    EXPECT_EQ(i.getConfig().number.decimal_symbol, ",");

    // Use the vector overload to switch to "en"
    i.setLocale(std::vector<std::string>{"en"});
    EXPECT_EQ(i.getConfig().number.decimal_symbol, ".");
}
