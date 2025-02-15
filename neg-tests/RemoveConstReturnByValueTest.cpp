#include "gtest/gtest.h"

/// Contains situations where removing the `const` when returning by value can break the user code

// #1 storing the returned value in a `auto&` variable
#ifndef BC_API_CHANGED
const std::string GetStr() {
    return "";
}
#else
std::string GetStr() {
    return "";
}
#endif  // !BC_API_CHANGED

TEST(RemoveConstReturnByValueTest_Neg, AutoRef) {
    auto& temp = GetStr();
    // Fixable by removing the referene
    //auto temp = GetStr();
    (void)temp;
}

// #2 The returned variable was a Base class, and also used inside a ternary operator
// where the other type was of type `const Derived`, and the Derived type had an implicit constructor from Base.
// After the `const` is removed we get error: incompatible types in ternary operator: ? `const Derived` : `Base`
struct Base {};
struct Derived : Base {
    Derived() = default;
    Derived(const Base& base) : Base(base) {}
};

void UseDerived(const Derived&) {}

#ifndef BC_API_CHANGED
const Base GetBase() {
    return {};
}
#else
Base GetBase() {
    return {};
}
#endif  // !BC_API_CHANGED

TEST(RemoveConstReturnByValueTest_Neg, TernaryOpMismatch) {
    const Derived d{};
    UseDerived(true ? d : GetBase());
    // Fixable by calling the Derived class constructor explicitly
    //UseDerived(true ? d : Derived{GetBase()});
}

// #3 now, ambiguous call
// GetVal().Get() returns a `const double&`,
// at least until we remove the `const` from the return type of `GetVal`
struct Val {
    double& Get() { return val; }
    const double& Get() const { return val; }

    double val;
};

#ifndef BC_API_CHANGED
const Val GetVal() {
    return {6};
}
#else
Val GetVal() {
    return {6};
}
#endif  // !BC_API_CHANGED

void Ambig(double&, bool = true) {}
void Ambig(double&, int = 0) {}
void Ambig(const double&) {}

TEST(RemoveConstReturnByValueTest_Neg, OverloadChanged) {
    Ambig(GetVal().Get());
    // Fixable by using `std::as_const`
    //AmbigPrint(std::as_const(GetVal().Get()));
}
