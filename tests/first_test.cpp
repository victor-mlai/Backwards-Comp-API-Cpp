#include "gtest/gtest.h"

template <typename T, typename V = void> constexpr bool is_incomplete = true;
template <typename T> constexpr bool is_incomplete<T, std::enable_if_t<sizeof(T)>> = false;

struct InitName;

TEST(Renames, ForwardDecl) {
    void fun(const InitName&);
    static_assert(is_incomplete<InitName>);
}

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
