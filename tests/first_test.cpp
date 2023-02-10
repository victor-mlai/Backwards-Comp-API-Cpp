

#include "gtest/gtest.h"

TEST(AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA, BasicAssertions) {
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(6* 6, 42);
}