#include "ReturnTypeChange.hpp"
#include "gtest/gtest.h"

TEST(ReturnTypeChange_Neg, ImplicitConversions) {
    // Storing the result into an int was possible before the API Change -> no error
    //
    // After the API Change, this should not compile since CheckPasswordResult
    // has the `operator bool` marked `explicit(!std::is_same_v<T, bool>)`
    const int success = CheckPassword("H20");
	
    ASSERT_EQ(success, 1);
}
