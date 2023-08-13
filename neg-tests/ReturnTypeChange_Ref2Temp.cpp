#include "ReturnTypeChange.hpp"
#include "gtest/gtest.h"

struct StruktWrapper {
    // if Get return type is changed to return by value, then this method
    //  would end up returning a reference to a temporary => compile error
    const float& GetMemF() const { return m_s.GetMemF(); }
    void SetMemF(const float& memF) { m_s.SetMemF(memF); }

private:
    Strukt m_s{};
};

int main()
{
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
