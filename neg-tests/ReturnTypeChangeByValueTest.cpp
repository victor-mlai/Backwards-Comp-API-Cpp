#include "ReturnTypeChangeByValue.hpp"
#include "gtest/gtest.h"

struct OtherStrukt
{
	explicit OtherStrukt(float) {}
};

TEST(ReturnTypeChangeByValue_Neg, Auto) {
    Strukt obj;
	
	// After the API Change, x is of type detail::GetterRetT
	auto x = obj.GetMemF();

	// Constructor is explicit => cannot do the implicit conversion
	OtherStrukt otherObj(x);
}
