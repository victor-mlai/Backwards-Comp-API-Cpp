#include "NamespaceRename.hpp"
#include "gtest/gtest.h"

namespace v1_alias = path::to::v1;

void Fun(path::to::v1::SomeEnum val = path::to::v1::A) {
    (void)val;
}

void Foo(v1_alias::SomeEnum val = v1_alias::A) {
    ::Fun(val);
}

TEST(NamespaceRename, Basic) {
    path::to::v1::Bar obj;
    obj = v1_alias::Bar{};

    static_assert(sizeof(obj) == 1);
    Fun(obj);
    path::to::v1::Fun();

    Foo();
    ::Fun(path::to::v1::B);

    // Test that the new method also works
#ifdef BC_API_CHANGED
    path::to::v2::Bar obj_v2;
    obj_v2 = v1_alias::Bar{};
    path::to::v2::Fun(obj_v2);
    ::Fun(path::to::v2::B);
#endif
}
