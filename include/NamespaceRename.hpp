#pragma once

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

// then add a namespace alias
namespace path::to {
    namespace v1 = path::to::v2;
}
#endif
