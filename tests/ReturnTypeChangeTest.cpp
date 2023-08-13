#include "ReturnTypeChange.hpp"
#include "gtest/gtest.h"

TEST(ReturnTypeChange, BasicAssertions) {
    // ----- primitive `T` changed to `NewUserDefT` -----
    // Testing calls to TryFoo
    {
        ASSERT_FALSE(TryFoo(false));
        ASSERT_TRUE(TryFoo(true) ? true : false);

        const bool succ = TryFoo(true);
        ASSERT_TRUE(succ);

        // Test that the new return type works
#ifndef OLD_CODE_ENABLED
        TryFooResult res = TryFoo(false);
        ASSERT_TRUE(res.m_errMsg.has_value());

        auto res_auto = TryFoo(false);
        ASSERT_FALSE(res_auto); // using the implicit cast
#endif
    }

    //------ primitive `const T&` changed to primitive `T` -----------
    // Testing calls of a mutable object
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

    // Testing calls of an immutable object
    {
        const Strukt s;
        int iVal = static_cast<int>(s.GetMemF());
        float val = s.GetMemF();
        const float& constValRef = s.GetMemF();

        ASSERT_EQ(iVal, 3);
        ASSERT_FLOAT_EQ(val, 3.f);
        ASSERT_FLOAT_EQ(constValRef, 3.f);
    }
}
