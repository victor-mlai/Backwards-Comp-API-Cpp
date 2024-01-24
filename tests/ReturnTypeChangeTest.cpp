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

// The return type changed from primitive `const T&` to primitive `T`
// - primitive means int, float, bool etc.
TEST(ReturnTypeChange, ConstRef2ByValue) {
    Strukt s_mut;
    const float constVal = s_mut.GetMemF();
    const float& constValRef = s_mut.GetMemF();
    auto&& refRef = s_mut.GetMemF();
    auto& constRef = s_mut.GetMemF();
    const int iVal = static_cast<int>(s_mut.GetMemF());
    s_mut.SetMemF(s_mut.GetMemF());

    ASSERT_EQ(iVal, 3);
    ASSERT_FLOAT_EQ(constVal, 3.f);
    ASSERT_FLOAT_EQ(constValRef, 3.f);
    ASSERT_FLOAT_EQ(refRef, 3.f);
    ASSERT_FLOAT_EQ(constRef, 3.f);
}

// Testing for dangling references in case Strukt was wrapped
TEST(ReturnTypeChange, NoDanglingReferences) {
    struct StruktWrapper {
        const float& GetMemF() const { return m_s.GetMemF(); }
        void SetMemF(const float& memF) { m_s.SetMemF(memF); }

    private:
        Strukt m_s{};
    };

    StruktWrapper s_mut;
    const float constVal = s_mut.GetMemF();
    const float& constValRef = s_mut.GetMemF();
    s_mut.SetMemF(s_mut.GetMemF());

    ASSERT_FLOAT_EQ(constVal, 3.f);
    ASSERT_FLOAT_EQ(constValRef, 3.f);
}
