#include "MethodDefaultParams.hpp"
#include "gtest/gtest.h"

TEST(StructRename, BasicAssertions) {
    SomeMethod(7);
    SomeMethod(7, true);
    SomeMethod(7, false, 1e-4f);

    // Test that the new method also works
#ifndef OLD_CODE_ENABLED
    SomeMethod(7, SomeMethodOpts{true});
    SomeMethod(7, SomeMethodOpts{.opt2 = 1e-4f});
    SomeMethod(7, SomeMethodOpts{.opt1 = true, .opt2 = 1e-4f});
#endif
}
