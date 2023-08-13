#pragma once
#include <optional>
#include <string>
#include <utility>

// ----- primitive `T` changed to `NewUserDefT` -----

#ifdef OLD_CODE_ENABLED
// This method returns true if it succeeds, otherwise false.
// Change: Make this method return some error message as well
//  so the user knows why it failed (returned false)
bool TryFoo(bool returnSuccess) {
    if (!returnSuccess)
        return false;

    return true;
}
#else
// Solution: return a new type that can be implicitly converted to bool.
struct TryFooResult {
    // can be deprecated, to let the users know FailingMethod can return a message now.
    operator bool() const { return !m_errMsg.has_value(); }

    std::optional<std::string> m_errMsg;
};

/// This method now returns an error message when it fails.
TryFooResult TryFoo(bool returnSuccess) {
    if (!returnSuccess)
        return {"shouldSucceed was false"};

    return {};
}
#endif


//------ primitive `const T&` changed to primitive `T` -----------

#ifdef OLD_CODE_ENABLED
// Change: Returning primitive types as const& is bad practice so change `const float&` to just `float`
// But, we cannot overload a function by return type and then deprecate it.
struct Strukt {
    const float& GetMemF() const { return m_memF; }
    void SetMemF(const float& memF) { m_memF = memF; }

private:
    float m_memF = 3.f;
};
#else
// Solution: same as before but with 1 extra cast operator
struct //[[deprecated("Store a NewRetT instead")]] - so users don't just `auto x = TryFoo();`
    GetterRetT
{
    using NewRetT = float;
    using OldRetT = const float&;

    GetterRetT(NewRetT newVal) : m_newVal(newVal) {}

    template <int = 0>  // Tells the compiler to prefer the other overload when:
    // conversion from 'GetterRetT' to 'float' is ambiguous
    //[[deprecated]] // deprecation message will say:
    // 'operator const float &' has been explicitly marked deprecated here
    operator OldRetT() const {
        return m_newVal;
    }

    operator NewRetT() const { return m_newVal; }

    NewRetT m_newVal;
};

struct Strukt {
    // return this by const& to avoid dangling references from StruktWrapper's Getter
    const GetterRetT& GetMemF() const { return m_memF; }
    void SetMemF(const float& memF) { m_memF.m_newVal = memF; }

private:
    GetterRetT m_memF = 3.f;
};
#endif
