#pragma once

#ifndef BC_API_CHANGED
// rename v1 to v2
namespace path::to::v1 {
    struct Bar {};
    constexpr int VAL = 42;
    enum SomeEnum { A, B, C };
    void Fun(const Bar& val = Bar()) {
        (void)val;
    }
}  // namespace path::to::v1
#else
// Add these on top for better visibility
namespace path::to::v2 {}

// Add a namespace alias
namespace path::to {
    namespace v1 = path::to::v2;
}

// Then simply rename
namespace path::to::v2 {
    struct Bar {};
    constexpr int VAL = 42;
    enum SomeEnum { A, B, C };
    void Fun(const Bar& val = Bar()) {
        (void)val;
    }
}  // namespace path::to::v2
#endif
