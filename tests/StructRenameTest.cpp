#include "StructRename.hpp"
#include "gtest/gtest.h"

void f(const OldName& val = OldName()) {
    (void)val;
}

struct Derived : OldName {
    Derived() : OldName() {}
};

TEST(StructRename, BasicAssertions) {
    OldName obj;
    obj = OldName{};

    OldName obj2(obj);
    OldName obj3 = std::move(obj2);

    static_assert(sizeof(obj) == 1);
    static_assert(sizeof(obj2) == 1);

    f(obj3);
    f();

    Derived d{};
    f(d);

    OldName sliced = d;
    f(sliced);

    // Test that the new struct name works
#ifndef OLD_CODE_ENABLED
    NewName newObj;
    newObj = NewName{};
    f(Derived());
    static_assert(sizeof(newObj) == 1);
#endif
}
