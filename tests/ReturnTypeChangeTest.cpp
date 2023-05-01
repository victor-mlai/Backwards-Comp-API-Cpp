#include "ReturnTypeChange.hpp"
#include "gtest/gtest.h"

TEST(ReturnTypeChange, BasicAssertions) {
    ASSERT_TRUE(FailingMethod(true));
    ASSERT_TRUE(FailingMethod(true) ? true : false);
    ASSERT_EQ(FailingMethod(true), true);
    ASSERT_EQ(FailingMethod(false), false);

    const bool succ = FailingMethod(true);
    ASSERT_TRUE(succ);

    const bool fail = FailingMethod(false);
    ASSERT_TRUE(!fail);

    {
        Strukt s;
        int iVal = static_cast<int>(s.GetMemF());
        float val = s.GetMemF();
        s.SetMemF(s.GetMemF());
        float& valRef = s.GetMemF();
        const float& constValRef = s.GetMemF();

        ASSERT_EQ(iVal, 3);
        ASSERT_FLOAT_EQ(val, 3.f);
        ASSERT_FLOAT_EQ(valRef, 3.f);
        ASSERT_FLOAT_EQ(constValRef, 3.f);
    }

    {
        struct StruktWrapper {
            // if Get return type is changed to return by value, then this method
            //  would end up returning a reference to a temporary => compile error
            float& GetMemF() { return m_s.GetMemF(); }
            const float& GetMemF() const { return m_s.GetMemF(); }
            void SetMemF(const float& memF) { m_s.SetMemF(memF); }

        private:
            Strukt m_s{};
        };

        StruktWrapper s;
        int iVal = static_cast<int>(s.GetMemF());
        float val = s.GetMemF();
        s.SetMemF(s.GetMemF());
        float& valRef = s.GetMemF();
        const float& constValRef = s.GetMemF();

        ASSERT_EQ(iVal, 3);
        ASSERT_FLOAT_EQ(val, 3.f);
        ASSERT_FLOAT_EQ(valRef, 3.f);
        ASSERT_FLOAT_EQ(constValRef, 3.f);
    }

    {
        const Strukt s;
        int iVal = static_cast<int>(s.GetMemF());
        float val = s.GetMemF();
        // s.SetMemF(s.GetMemF());
        // float& valRef = s.GetMemF();
        const float& constValRef = s.GetMemF();

        ASSERT_EQ(iVal, 3);
        ASSERT_FLOAT_EQ(val, 3.f);
        // ASSERT_FLOAT_EQ(valRef, 3.f);
        ASSERT_FLOAT_EQ(constValRef, 3.f);
    }

    {
        struct StruktWrapper {
            // if Get return type is changed to return by value, then this method
            //  would end up returning a reference to a temporary => compile error
            float& GetMemF() { return m_s.GetMemF(); }
            const float& GetMemF() const { return m_s.GetMemF(); }
            void SetMemF(const float& memF) { m_s.SetMemF(memF); }

        private:
            Strukt m_s{};
        };

        const StruktWrapper s;
        int iVal = static_cast<int>(s.GetMemF());
        float val = s.GetMemF();
        // s.SetMemF(s.GetMemF());
        // float& valRef = s.GetMemF();
        const float& constValRef = s.GetMemF();

        ASSERT_EQ(iVal, 3);
        ASSERT_FLOAT_EQ(val, 3.f);
        // ASSERT_FLOAT_EQ(valRef, 3.f);
        ASSERT_FLOAT_EQ(constValRef, 3.f);
    }
}
