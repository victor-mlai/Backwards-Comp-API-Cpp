#pragma once

#ifdef OLD_CODE_ENABLED
    // Todo: make this method receive a struct with the default parameters
    void SomeMethod(
        const int mandatory,
        const bool opt1 = false,
        const float opt2 = 1e-6
    ) {}
#else
    // Solution: If you just add the new SomeMethod, calling SomeMethod with
    // just the mandatory parameters becomes ambiguous. The compiler won't
    // know which of the 2 methods to choose. To tell it to prefer the newer
    // one we need to make the old one less specialized by making it a template
    template<int = 0>
    void SomeMethod(
        const int mandatory,
        const bool opt1 = false,
        const float opt2 = 1e-6f
    ) {}

    struct SomeMethodOpts { bool opt1 = false; float opt2 = 1e-6f; };
    void SomeMethod(
        const int mandatory,
        SomeMethodOpts opts = {}
    ) {}
#endif
