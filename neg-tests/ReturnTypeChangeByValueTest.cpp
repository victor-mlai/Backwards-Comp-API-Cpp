#include "ReturnTypeChangeByValue.hpp"
#include "gtest/gtest.h"

#include <type_traits> // std::is_same_v

template <typename T>
void Foo(T) requires std::is_same_v<T, float> {}

TEST(ReturnTypeChangeByValue_Neg, Auto) {
    Strukt obj;
	
	// After the API Change, x is of type `detail::GetterRetT`
	auto x = obj.GetMemF();

	// Error: no overload found that accepts `detail::GetterRetT`
	Foo(x);
}
