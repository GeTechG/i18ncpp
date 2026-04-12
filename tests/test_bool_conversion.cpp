#include <gtest/gtest.h>
#include <i18ncpp.h>

using namespace i18n;

TEST(BoolConversion, TrvSerializesBoolAsText) {
    I18N i;
    json data = {
        {"en", {
            {"status", "Value is {0}"}
        }}
    };
    i.load(data);
    i.setLocale("en");

    EXPECT_EQ(i.trv("status", true), "Value is true");
    EXPECT_EQ(i.trv("status", false), "Value is false");
}

TEST(BoolConversion, TrPluralvSerializesBoolAsText) {
    I18N i;
    json data = {
        {"en", {
            {"items", {
                {"one", "{0} item, active: {1}"},
                {"other", "{0} items, active: {1}"}
            }}
        }}
    };
    i.load(data);
    i.setLocale("en");

    EXPECT_EQ(i.trPluralv("items", 3, true), "3 items, active: true");
    EXPECT_EQ(i.trPluralv("items", 1, false), "1 item, active: false");
}

TEST(BoolConversion, IntegersStillWork) {
    I18N i;
    json data = {
        {"en", {
            {"msg", "Count: {0}"}
        }}
    };
    i.load(data);
    i.setLocale("en");

    EXPECT_EQ(i.trv("msg", 42), "Count: 42");
}
