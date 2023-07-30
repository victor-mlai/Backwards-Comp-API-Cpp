#include "ReturnTypeChange.hpp"
#include "gtest/gtest.h"

TEST(ReturnTypeChange, BasicAssertions) {
    // Testing calls to SomeMethod
    {
        ASSERT_TRUE(SomeMethod(true));
        ASSERT_TRUE(SomeMethod(true) ? true : false);
        ASSERT_EQ(SomeMethod(true), true);
        ASSERT_EQ(SomeMethod(false), false);

        const bool succ = SomeMethod(true);
        ASSERT_TRUE(succ);

        const bool fail = SomeMethod(false);
        ASSERT_TRUE(!fail);
    }

    // Testing calls of a mutable object
    {
        Strukt s_mut;
        int iVal = static_cast<int>(s_mut.GetMemF());
        float val = s_mut.GetMemF();
        s_mut.SetMemF(s_mut.GetMemF());
        float& valRef = s_mut.GetMemF();
        const float& constValRef = s_mut.GetMemF();

        ASSERT_EQ(iVal, 3);
        ASSERT_FLOAT_EQ(val, 3.f);
        ASSERT_FLOAT_EQ(valRef, 3.f);
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
