#include "gtest/gtest.h"

#ifdef OLD_CODE_ENABLED
// Todo: rename InitName to FinalName
struct InitName {};
#else
// simply rename
struct FinalName {};

// then add this type alias
using InitName = FinalName;
#endif

// TODO: test that forward declarations fail to compile when OLD_CODE_ENABLED is OFF
// struct InitName;

void f(const InitName& val = InitName()) {
	(void)val;
}

struct UserDerived : InitName {
	UserDerived() : InitName() {}

};

TEST(Renames, BasicAssertions) {
	InitName obj;
	obj = InitName{};

	InitName obj2(obj);
	InitName obj3 = std::move(obj2);

	static_assert(sizeof(obj) == 1);
	static_assert(sizeof(obj2) == 1);

	f(obj3);
	f();

	UserDerived usd{};
	f(usd);

	InitName sliced = usd;
	f(sliced);
}
