#pragma once
#include <optional>
#include <string>
#include <utility>

// Use when changing the return type from primitive `T` to `NewUserDefT`
#pragma region Change primitive T to NewUserDefT

#ifndef BC_API_CHANGED
// This method returns true if it succeeds, otherwise false.
// Change: Make this method return some error message as well
//  so the user knows why it failed (returned false)
bool CheckPassword(std::string pass) {
    if (pass.empty())
        return false;

    if (pass.starts_with("1234"))
        return false;

    return true;
}
#else
// Solution: return a new type that can be implicitly converted to bool.
struct CheckPasswordResult {
    /// explicit(!std::is_same_v<T, bool>) means it implicitly converts to bool only.
    template <typename T>
    explicit(!std::is_same_v<T, bool>) operator T() const {
        return !errMsg.has_value();
    }

    /// If you need to allow `int x = CheckPassword();` then simply use this one:
    //operator bool() const { return !errMsg.has_value(); }

    std::optional<std::string> errMsg;
};

/// This method now returns an error message when it fails.
CheckPasswordResult CheckPassword(std::string pass) {
    if (pass.empty())
        return {"Password must not be empty."};

    if (pass.starts_with("1234"))
        return {"Password not strong enough."};

    return {std::nullopt};
}
#endif

#pragma endregion

// Use when changing the return type from primitive `const T&` to primitive `T`
#pragma region Change primitive const T& to primitive T

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

    template <int = 0>  // Makes this less specialized so the compiler prefers the other
                        // overload when the conversion from 'GetterRetT' to 'float' is ambiguous
    // [[deprecated]] // deprecation message will say:
    // 'operator const float &' has been explicitly marked deprecated here
    operator OldRetT() const {
        return m_newVal;
    }

    operator NewRetT() const { return m_newVal; }

private:
    NewRetT m_newVal;
};

struct Strukt {
    // return this by const& to avoid dangling references when called from wrapper cllases like
    // StruktWrapper::GetMemF
    const GetterRetT& GetMemF() const { return m_memF; }
    void SetMemF(const float& memF) { m_memF = GetterRetT{memF}; }

private:
    GetterRetT m_memF = GetterRetT{3.f};
};
#endif

#pragma endregion
