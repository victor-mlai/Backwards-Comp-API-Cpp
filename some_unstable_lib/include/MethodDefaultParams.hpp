#pragma once

#ifndef BC_API_CHANGED
    // Change: make this method receive a struct with the default parameters
    void SomeMethod(
        int /*mandatory*/,
        bool /*opt1*/ = false,
        float /*opt2*/ = 1e-6f,
        int /*opt3*/ = 42
    ) {}
#else
    // Solution: If you would just overload SomeMethod, calling SomeMethod with
    // just the mandatory parameters would be ambiguous. The compiler won't
    // know which of the 2 methods to choose. To tell it to prefer the newer
    // one we need to make the old one less specialized by making it a template
    template<int = 0>
    void SomeMethod(
        int /*mandatory*/,
        bool /*opt1*/ = false,
        float /*opt2*/ = 1e-6f,
        int /*opt3*/ = 42
    ) {}

    struct SomeMethodOpts {
        bool opt1 = false;
        float opt2 = 1e-6f;
        int opt3 = 42;
    };
    void SomeMethod(
        int /*mandatory*/,
        SomeMethodOpts /*opts*/ = {}
    ) {}
#endif
