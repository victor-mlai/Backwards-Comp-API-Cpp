#include "gtest/gtest.h"

#ifdef OLD_CODE_ENABLED
// rename v1 to v2
namespace path::to::v1 {
	struct Bar {};
	constexpr int VAL = 42;
	enum SomeEnum { A, B, C };
	void f(const Bar& val = Bar()) {
		(void)val;
	}
}  // namespace path::to::v1
#else
// simply rename
namespace path::to::v2 {
	struct Bar {};
	constexpr int VAL = 42;
	enum SomeEnum { A, B, C };
	void f(const Bar& val = Bar()) {
		(void)val;
	}
}  // namespace path::to::v2

// add a using namespace
//  in the old namespace
namespace path::to::v1 {
	using namespace path::to::v2;
}
#endif

using namespace path::to::v1;

// TODO: test that forward declarations fail to compile when OLD_CODE_ENABLED is
// OFF struct InitName;

void g(SomeEnum val = A) {
	(void)val;
}

TEST(Renames, NamespaceRename) {
	Bar obj;
	obj = Bar{};

	Bar obj2(obj);
	Bar obj3 = std::move(obj2);

	static_assert(sizeof(obj) == 1);
	static_assert(sizeof(obj2) == 1);

	f(obj3);
	f();

	g();
	g(B);
}
