#include "MethodDefaultParams.hpp"
#include "gtest/gtest.h"

TEST(StructRename, BasicAssertions) {
    SomeMethod(7);  // This calls SomeMethod(int, SomeMethodOpts{});
    SomeMethod(7, true);
    SomeMethod(7, false, 1e-4f);
    SomeMethod(7, false, 1e-4f, 21);

    // Test that the method also works with the new struct
#ifdef BC_API_CHANGED
    SomeMethod(7, SomeMethodOpts{true});
    SomeMethod(7, SomeMethodOpts{.opt2 = 1e-4f});
    SomeMethod(7, SomeMethodOpts{.opt3 = 21});
    SomeMethod(7, SomeMethodOpts{.opt1 = true, .opt2 = 1e-4f});
    SomeMethod(7, SomeMethodOpts{.opt2 = 1e-4f, .opt3 = 21});
    SomeMethod(7, SomeMethodOpts{.opt1 = true, .opt2 = 1e-4f, .opt3 = 21});
#endif
}
