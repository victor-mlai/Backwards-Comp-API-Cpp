#include "ReturnTypeChange.hpp"
#include "gtest/gtest.h"

// The return type changed from primitive `T` to `NewUserDefT`
// - primitive means int, float, bool etc.
TEST(ReturnTypeChange, Primitive2UserDefined) {
    ASSERT_FALSE(CheckPassword(""));
    if (CheckPassword("1234"))
        FAIL() << "Failed to CheckPassword in if statement";
    else if (!CheckPassword("H20"))
        FAIL() << "Failed to CheckPassword in if(!..) statement";

    // won't compile if CheckPasswordResult::operator bool() is explicit
    bool success = CheckPassword("H20");
    ASSERT_TRUE(success);

    // Test that the new API works
#ifdef BC_API_CHANGED
    CheckPasswordResult res = CheckPassword("");
    ASSERT_TRUE(res.errMsg.has_value());
    ASSERT_FALSE(res.errMsg.value().empty());

    const auto res_auto = CheckPassword("H20");
    ASSERT_FALSE(res_auto.errMsg.has_value());
#endif
}
