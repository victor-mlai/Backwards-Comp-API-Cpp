#include "MethodDefaultParams.hpp"
#include "gtest/gtest.h"

void f(const InitName& val = InitName()) {
    (void)val;
}

struct UserDerived : InitName {
    UserDerived() : InitName() {}
};

TEST(StructRename, BasicAssertions) {
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
