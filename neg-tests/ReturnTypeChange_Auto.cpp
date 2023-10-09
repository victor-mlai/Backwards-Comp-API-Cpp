#include "ReturnTypeChange.hpp"
#include "gtest/gtest.h"
TEST(ReturnTypeChange_Auto, AutoFailures) {
    //const Strukt s_mut;
    //const auto& val = s_mut.GetMemF();

    foo  // placeholder compile error.
    //// should have said that it cannot compare floats and GetterRetT.... but it doesn't
    //ASSERT_FLOAT_EQ(val, 3.f) << val;
}
