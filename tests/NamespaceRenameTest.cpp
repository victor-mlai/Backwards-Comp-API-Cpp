#include "NamespaceRename.hpp"
#include "gtest/gtest.h"

using namespace path::to::v1;

void g(SomeEnum val = A) {
    (void)val;
}

TEST(NamespaceRename, Basic) {
    path::to::v1::Bar obj;
    obj = Bar{};

    path::to::v1::Bar obj2(obj);
    Bar obj3 = std::move(obj2);

    static_assert(sizeof(obj) == 1);
    static_assert(sizeof(obj2) == 1);

    f(obj3);
    f();

    g();
    g(path::to::v1::B);
}
