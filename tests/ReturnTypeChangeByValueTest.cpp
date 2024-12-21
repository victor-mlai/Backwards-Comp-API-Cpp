#include "ReturnTypeChangeByValue.hpp"
#include "gtest/gtest.h"

// The return type changed from primitive `const T&` to primitive `T`
// - primitive means int, float, bool etc.
TEST(ReturnTypeChangeByValue, ConstRef2ByValue) {
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
TEST(ReturnTypeChangeByValue, NoDanglingReferences) {
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
