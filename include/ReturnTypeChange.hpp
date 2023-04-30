#pragma once
#include <optional>
#include <string>
#include <utility>

// ----- `bool` -> `optional` -----

#ifdef OLD_CODE_ENABLED
// Sometimes the user might want to know why a function fails,
// but without having to catch exceptions.
// Todo: make this method return an error message when it fails.

/// This method returns true if it succeeds, otherwise false.
bool FailingMethod(bool shouldFail) {
    if (shouldFail)
        return false;

    return true;
}
#else
// Solution: return a new type that can be implicitly converted to bool.
struct FailingMethodResult {
    // can be deprecated, to let the users know FailingMethod can return a message now.
    operator bool() const { return !m_errMsg.has_value(); }

    std::optional<std::string> m_errMsg;
};

/// This method now returns an error message when it fails.
FailingMethodResult FailingMethod(bool shouldFail) {
    if (shouldFail)
        return {"Parameter was true"};

    return {};
}
#endif


//------ `const float&` -> `float` -----------

#ifdef OLD_CODE_ENABLED
// Returning primitive types as const& is bad practice.
// But, we cannot overload a function by return type and then deprecate it.
// Todo: change `const float&` to just `float`
struct Strukt {
    float& GetMemF() { return m_memF; }
    const float& GetMemF() const { return m_memF; }
    void SetMemF(const float& memF) { m_memF = memF; }

private:
    float m_memF = 3.f;
};
#else
// Solution: same as before but with 1 addition
struct GetterRetT {
    using NewRetT = float;
    using OldRetT = const float&;

    GetterRetT(NewRetT newVal) : m_newVal(newVal) {}

    template <int = 0>  // Tells the compiler to prefer the other overload when:
    // conversion from 'GetterRetT' to 'float' is ambiguous
    //[[deprecated]] // deprecation message says:
    // 'operator const float &' has been explicitly marked deprecated here
    operator OldRetT() const {
        return m_newVal;
    }

    operator NewRetT() const { return m_newVal; }

    NewRetT m_newVal;
};

struct Strukt {
    float& GetMemF() { return m_memF.m_newVal; }
    // return this by const& to avoid dangling references from StruktWrapper's Getter
    const GetterRetT& GetMemF() const { return m_memF; }
    void SetMemF(const float& memF) { m_memF.m_newVal = memF; }

private:
    GetterRetT m_memF = 3.f;
};
#endif
