#pragma once
#include <optional>
#include <string>
#include <utility>

#pragma region Change primitive `T` to `NewUserDefT`

#ifndef BC_API_CHANGED
// This method returns true if it succeeds, otherwise false.
// Change: Make this method return some error message as well
//  so the user knows why it failed (returned false)
bool TryParseString(std::string num) {
    if (num.empty())
        return false;

    if (num[0] != 'k')
        return false;

    return true;
}
#else
// Solution: return a new type that can be implicitly converted to bool.
struct TryParseStringResult {
    // can be deprecated, to let the users know FailingMethod can return a message now.
    operator bool() const { return !m_errMsg.has_value(); }

    std::optional<std::string> m_errMsg;
};

/// This method now returns an error message when it fails.
TryParseStringResult TryParseString(std::string num) {
    if (num.empty())
        return {"string must not be empty."};

    if (num[0] != 'k')
        return {"string must start with the secret letter."};

    return {};
}
#endif

#pragma endregion

#pragma region Change primitive `const T&` to primitive `T`

#ifndef BC_API_CHANGED
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
struct  //[[deprecated("Store NewRetT instead")]] - so users don't just `auto x = obj.GetMemF();`
    GetterRetT {
    using NewRetT = float;
    using OldRetT = const float&;

    explicit GetterRetT(NewRetT newVal) : m_newVal(newVal) {}

    template <int = 0>  // Tells the compiler to prefer the other overload when
    // the conversion from 'GetterRetT' to 'float' is ambiguous
    //[[deprecated]] // deprecation message will say:
    // 'operator const float &' has been explicitly marked deprecated here
    operator OldRetT() const {
        return m_newVal;
    }

    operator NewRetT() const { return m_newVal; }

    NewRetT m_newVal;
};

struct Strukt {
    // return this by const& to avoid dangling references when called from wrapper cllases like
    // StruktWrapper::GetMemF
    const GetterRetT& GetMemF() const { return m_memF; }
    void SetMemF(const float& memF) { m_memF.m_newVal = memF; }

private:
    GetterRetT m_memF = GetterRetT{3.f};
};
#endif

#pragma endregion
