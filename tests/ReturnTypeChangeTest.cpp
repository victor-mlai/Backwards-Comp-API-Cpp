#include "ReturnTypeChange.hpp"
#include "gtest/gtest.h"

struct StruktWrapper {
    const float& GetMemF() const { return m_s.GetMemF(); }
    void SetMemF(const float& memF) { m_s.SetMemF(memF); }

private:
    Strukt m_s{};
};

TEST(ReturnTypeChange, BasicAssertions) {
    // ----- primitive `T` changed to `NewUserDefT` -----
    // Testing calls to TryParseString
    {
        ASSERT_FALSE(TryParseString(""));
        if (TryParseString(""))
            FAIL() << "Failed TryParseString in if statement";
        else if (!TryParseString("k"))
            FAIL() << "Failed TryParseString in if(!..) statement";

        // won't compile if TryParseStringResult::operator bool() is explicit
        const bool succ = TryParseString("key");
        ASSERT_TRUE(succ);

        // Test that the new return type works
#ifdef BC_API_CHANGED
        TryParseStringResult res = TryParseString("");
        ASSERT_TRUE(res.m_errMsg.has_value());
        ASSERT_FALSE(res.m_errMsg.value().empty());

        const auto res_auto = TryParseString("key");
        ASSERT_FALSE(res_auto.m_errMsg.has_value());
#endif
    }

    //------ primitive `const T&` changed to primitive `T` -----------
    // Testing calls on a mutable object
    {
        Strukt s_mut;
        int iVal = static_cast<int>(s_mut.GetMemF());
        float val = s_mut.GetMemF();
        s_mut.SetMemF(s_mut.GetMemF());
        const float& constValRef = s_mut.GetMemF();

        ASSERT_EQ(iVal, 3);
        ASSERT_FLOAT_EQ(val, 3.f);
        ASSERT_FLOAT_EQ(constValRef, 3.f);
    }

    // Testing calls on an immutable object
    {
        const Strukt s;
        int iVal = static_cast<int>(s.GetMemF());
        float val = s.GetMemF();
        const float& constValRef = s.GetMemF();

        ASSERT_EQ(iVal, 3);
        ASSERT_FLOAT_EQ(val, 3.f);
        ASSERT_FLOAT_EQ(constValRef, 3.f);
    }

    StruktWrapper s_mut;
    {
        int iVal = static_cast<int>(s_mut.GetMemF());
        float val = s_mut.GetMemF();
        s_mut.SetMemF(s_mut.GetMemF());
        const float& constValRef = s_mut.GetMemF();

        ASSERT_EQ(iVal, 3);
        ASSERT_FLOAT_EQ(val, 3.f);
        ASSERT_FLOAT_EQ(constValRef, 3.f);
    }

    {
        const StruktWrapper s;
        int iVal = static_cast<int>(s.GetMemF());
        float val = s.GetMemF();
        s_mut.SetMemF(s.GetMemF());
        const float& constValRef = s.GetMemF();

        ASSERT_EQ(iVal, 3);
        ASSERT_FLOAT_EQ(val, 3.f);
        ASSERT_FLOAT_EQ(constValRef, 3.f);
    }
}
