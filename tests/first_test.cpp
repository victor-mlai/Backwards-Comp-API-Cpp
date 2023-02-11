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

TEST(Renames, BasicAssertions) {
    InitName obj;
    obj = InitName{};

    static_assert(sizeof(obj) == 1);
}
