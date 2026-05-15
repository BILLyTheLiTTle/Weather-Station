#include <gtest/gtest.h>
#include "Formatter.h"

TEST(FormatterTest, DateFormatIsCorrect) {
    char buffer[20];
    // Test: 15 Μαΐου 2026, 10:30
    formatDateTime(buffer, 15, 5, 2026, 10, 3);
    EXPECT_STREQ("15/05/2026 10:03", buffer);
}

TEST(FormatterTest, PositiveNumberFormatWithTwoDecimals) {
    char buffer[10];
    formatNumber(buffer, (int16_t)1234);
    EXPECT_STREQ("12.34", buffer);
}

TEST(FormatterTest, NegativeeNumberFormatWithTwoDecimals) {
    char buffer[10];
    formatNumber(buffer, (int16_t)-1234);
    EXPECT_EQ(std::string("-12.34"), std::string(buffer));
}

TEST(FormatterTest, PositiveNumberFormatWithOneDecimal) {
    char buffer[10];
    formatNumber(buffer, (int16_t)1203);
    EXPECT_STREQ("12.03", buffer);
}
