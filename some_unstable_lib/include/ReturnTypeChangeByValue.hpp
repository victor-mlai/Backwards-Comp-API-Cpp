#pragma once
#include <optional>
#include <string>
#include <utility>

// Changing the return type from primitive `const T&` to primitive `T`
#ifndef BC_API_CHANGED
// Change: Since returning primitive types as const& is bad practice, change `const float&` to just `float`
struct Strukt {
    const float& GetMemF() const { return m_memF; }
    void SetMemF(const float& memF) { m_memF = memF; }

private:
    float m_memF = 3.f;
};
#else
// We cannot overload a function by return type and then deprecate it.
// Solution: same as "ReturnTypeChange" but with 1 extra cast operator
namespace detail
{
struct  //[[deprecated("Use NewRetT instead")]] - inform users about the change
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
}

struct Strukt {
    // return this by const& to avoid dangling references when called from wrapper cllases like
    // SomeStruktWrapper::GetMemF
    const detail::GetterRetT& GetMemF() const { return m_memF; }
    void SetMemF(const float& memF) { m_memF = detail::GetterRetT{memF}; }

private:
    detail::GetterRetT m_memF = detail::GetterRetT{3.f};
};
#endif
