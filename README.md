# Making C++ API changes Backwards Compatible

This project contains "tricks" on how to make backwards compatible API changes.
(Eg. renaming a class, changing parameter types, converting an enum to a variant etc.)

It contains tests (see /tests) to ensure that
these tricks are indeed backwards compatible and showcase what cases (if any) will they break.

Some of them also have negative tests (see /neg-tests) to showcase some rare cases where the
API change breaks code that previously compiled, even with the "trick".

# Contributing

This project just got created.

Please feel free to create Issues and Pull Requests to improve this list.

# Warning

These tricks assume the users of your API don't require ABI compatibility, or use
forward declarations or function pointer aliases for your types
(In general, they shouldn't forward declare foreign types).

# Project internals

/some_unstable_lib contains a header file for each API change in the list further down.
Each header either exposes the old API or the changed one based on the macro `BC_API_CHANGED`.

/tests represents the users of the library whose code must compile
before and after the API change.

/neg-tests contains code that should not compile after the API change.
One file per each such case.

All tests are run twice:

1. with the macro `BC_API_CHANGED` OFF (before the API change)
2. and ON (after).

.github/workflows/deploy.yml pushes new changes to this README to Github Pages.


# Overview
* [Rename a namespace](#mv_namespace)
* [Rename a type](#mv_type)
* [Rename a header](#mv_header)
* [Change default parameters](#change_defaults)
* [Change the return type](#change_ret_type)
* [Reasonably safe changes](#reasonably_safe_changes)
<!--
* [Move symbols to a different namespace](#move_symb_to_ns)
* [Move symbols to a different class](#move_symb_to_class)
-->


# How to:

<a id="mv_namespace"></a>

## Rename a namespace

### Initial code:

```cpp
namespace path::to::v1 { ... }
```

### Scenario:

We maybe need to change the namespace name to fix a typo.
We will change it from `path::to::v1` to `path::to::v2`.

### Solution:

Rename the old namespace to the new one and add a namespace alias
for the old one.

```diff
+ namespace path::to::v2 {}
+ namespace path::to {
+   namespace v1 = path::to::v2;
+ }
+ 
- namespace path::to::v1 { ... }
+ namespace path::to::v2 { ... }
```

### Remarks:

* `[[deprecated]]` attribute doesn't work on namespace aliases.
  You can try compiler specific directives (Eg. `#pragma deprecated(keyword)` for
  msvc)

* The empty namespace `namespace path::to::v2 {}` was added at the top of the
file for visibility purposes

### Relevant Files:

* API: [NamespaceRename.hpp](some_unstable_lib/include/NamespaceRename.hpp)
* User: [NamespaceRenameTest.cpp](tests/NamespaceRenameTest.cpp)


<a id="mv_type"></a>

## Rename a type

### Initial code:

```cpp
struct OldName { ... };
```

### Scenario:

We maybe need to update the struct name to fix a typo.
We will change it to `NewName`.

### Solution:

We can use a type alias.

```diff
- struct OldName { ... };
+ struct NewName { ... };
+ using OldName = NewName;
```

### Remarks:

* You can deprecate the old `OldName`.
* The users might learn the hard way that they shouldn't forward declare foreign
  types.

### Relevant Files:

* API: [StructRename.hpp](some_unstable_lib/include/StructRename.hpp)
* User: [StructRenameTest.cpp](tests/StructRenameTest.cpp)


<a id="mv_header"></a>

## Rename a header

### Initial code:

```cpp
// v1/OldName.hpp:
...
```

### Scenario:

We need to rename the header to `v2/NewName.hpp`.

### Solution:

1. Rename the header:

```diff
- // v1/OldName.hpp:
+ // v2/NewName.hpp:
...
```

2. Create a compatibility header file in the old location that includes
  the renamed one.

```cpp
// v1/OldName.hpp: <- created to only include the renamed header + deprecation notice
#include "v2/NewName.hpp"

// You can also deprecate it by inserting a compilation warning:
// #warning OldName.hpp is deprecated, include "v2/NewName.hpp".`
// Don't use #error since there is no way for users to silence it.
```

### Remarks:

Rename using your versioning tool (Git/SVN) so you don't lose blame history.
For Git, do the change 2 steps in 2 different commits.


<a id="change_defaults"></a>

## Change default parameters

### Initial code:

```cpp
void SomeMethod(
    int mandatory,
    bool opt1 = false,
    float opt2 = 1e-6f,
    int opt3 = 42
) { ... }
```

### Scenario:

This method receives too many default parameters, and it only becomes
harder for users to call it with only 1 or 2 parameters changed. We need to
change the method to receive a struct containing these parameters instead.

### Solution:

If you would just overload `SomeMethod` with the default parameters changed,
users calling `SomeMethod` with just the mandatory parameters will now have the
compiler complain about ambiguity (that it doesn't know which of the 2 methods to call).

To tell the compiler to prefer the newer method we need to make the old one less
specialized by making it a template.

```diff
+ template<int = 0>
void SomeMethod(
    int mandatory,
    bool opt1 = false,
    float opt2 = 1e-6f,
    int opt3 = 42
+ ) {
+  // Call the new implementation now
+  SomeMethod(mandatory, SomeMethodOpts{opt1, opt2, opt3});
+ }
+ 
+ struct SomeMethodOpts { bool opt1 = false; float opt2 = 1e-6; int opt3 = 42; };
+ void SomeMethod(
+     int mandatory,
+     SomeMethodOpts opts = {}
) { ... }
```

### Remarks:

 * You can deprecate the old `SomeMethod` (now a template)
 * If the definition needs to be in the .cpp file, and the function is dll exported, you need to explicitly instantiate the templated `SomeMethod` in the .cpp file
```cpp
template<> DLL_EXPORT void SomeMethod<0>(
    int mandatory, bool opt1 = false, float opt2 = 1e-6f, int opt3 = 42);
```

### Relevant Files:

* API: [MethodDefaultParams.hpp](some_unstable_lib/include/MethodDefaultParams.hpp)
* User: [MethodDefaultParamsTest.cpp](tests/MethodDefaultParamsTest.cpp)


<a id="change_ret_type"></a>

## Change the return type

### Warning:

Prefer to just add a new method called slightly different instead.
What's about to follow is over-engineered.

In short: we will overload the implicit cast operator of the returned type,
and if the returned type is a primitive, we will create a new type that wraps it.

### Initial code:

```cpp
// (1) change some primitive `T` to `NewUserDefT`
bool CheckPassword(std::string);

// (2) change some primitive `const T&` to primitive `T`
struct Strukt {
  const float& GetMemF() const { return m_memF; }
private:
  float m_memF;
};
```

### Scenario:

(1) `CheckPassword` method returns true if it succeeds, otherwise false.
Make this method return some meaningful error message so the user knows why it
failed (why it returned false).

(2) `Strukt::GetMemF` returns a primitive type as const& which is bad for multiple reasons
(performance, [lifetime](https://www.sandordargo.com/blog/2020/11/18/when-use-const-3-return-types#returning-const-references), complexity issues).
We need to return by value.

Unfortunately, we cannot just overload a function by return type and then deprecate it.

### Solution:

For situation (1): Add `operator bool()` so that the new type can be implicitly casted to `bool`.

```diff
// (1) change primitive `T` to `NewUserDefT`
+ struct CheckPasswordResult { // mimics std::expected<void, std::string>
+     operator bool() const { return !m_errMsg.has_value(); }
+     const std::string& error() const { return m_errMsg.value(); }
+ private:
+     std::optional<std::string> m_errMsg;
+ };
- bool CheckPassword(std::string);
+ CheckPasswordResult CheckPassword(std::string);
```

- (1.1): If you want to take it a step further, and allow implicit casts only to `bool`,
since C++20 you can make the cast operator conditionally explicit
(In the tests, `int x = CheckPassword("");` doesn't compile after the API change,
while `bool x = CheckPassword("");` does. See [neg-tests/ReturnTypeChangeTest.cpp](neg-tests/ReturnTypeChangeTest.cpp))

For situation (2): Add a new class `GetterRetT` with 2 implicit cast operators to `NewRetT` and to `OldRetT`.
"Mark" the implicit cast operator to `OldRetT` as deprecated and as "less specialized"
(i.e. as template, so that the compiler will choose at "overload resolution" the `NewRetT` overload).

Additionally, inside the `Strukt` return `GetterRetT` by `const&` so that we avoid runtime
exceptions from dangling references in user's code in case they have a StruktWrapper class that
also has a `const float& GetMemF()` that called and returned the result of our `GetMemF()`.

```diff
// (2) change primitive `const T&` to primitive `T`
+ struct GetterRetT {
+   template <int = 0> // (2.1)
+   operator OldRetT () const { ... }
+   operator NewRetT () const { ... }
+ };

struct Strukt {
-   const float& GetMemF() const { return m_memF; }
+   const GetterRetT& GetMemF() const { return m_memF; }
private:
-   float m_memF = 3.f;
+   GetterRetT m_memF = 3.f;
};
```

### Relevant Files:

* API: [include/ReturnTypeChange.hpp](some_unstable_lib/include/ReturnTypeChange.hpp)
  [include/ReturnTypeChangeByValue.hpp](some_unstable_lib/include/ReturnTypeChangeByValue.hpp)
* User: [tests/ReturnTypeChangeTest.cpp](tests/ReturnTypeChangeTest.cpp)
  [tests/ReturnTypeChangeByValueTest.hpp](tests/ReturnTypeChangeByValueTest.hpp)
* Neg: [neg-tests/ReturnTypeChangeTest.cpp](neg-tests/ReturnTypeChangeTest.cpp)


<a id="change_to_enum_class"></a>

## Change old-style enum to enum class

### Warning:

Changing the enum to enum class will inherently breaks implicit conversions
to integers (e.g. when the enum is used as bit flags: `STYLE_BOLD | STYLE_ITALLIC` results in a `int`).

### Initial code:

```cpp
enum Style {
    STYLE_BOLD,
    STYLE_ITALLIC,
    STYLE_STRIKE_THROUGH,
};
```

### Scenario:

We need to modernize the API to use `enum class` instead.

### Solution:

In order to not break scoped uses of the enum (e.g. `auto style = Style::STYLE_BOLD`)
we will duplicate the enum fields with the enum class's naming style,
and make sure their value is assigned to the old enum fields.

In order to not break unscoped uses of the enum (e.g. `auto style = STYLE_BOLD`),
we will define static variables for each enum entry.

```diff
- enum Style {
+ enum class Style {
+   Bold,
+   Itallic,
+   StrikeThrough,
-   STYLE_BOLD,
-   STYLE_ITALLIC,
-   STYLE_STRIKE_THROUGH,
+   STYLE_BOLD = Bold,
+   STYLE_ITALLIC = Itallic,
+   STYLE_STRIKE_THROUGH = StrikeThrough,
};

+ static inline Style STYLE_BOLD = Style::Bold;
+ static inline Style STYLE_ITALLIC = Style::Itallic;
+ static inline Style STYLE_STRIKE_THROUGH = Style::StrikeThrough;
```

### Remarks: 

* Inspired by the [memory_order change in the standard](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0439r0.html)

* If the enum was used as bit flags, define bitwise operators as well.
And if there were methods that recieved the unscoped enum as `int`,
overload them to receive the scoped enum now (since the bitwise operators return the scoped enum,
if they were to return an `int`, users will not be able to chain more than 2 of them: e.g.
`Print(STYLE_BOLD | STYLE_ITALLIC | STYLE_STRIKE_THROUGH) // operator|(Style, int) is not overloaded`).

```cpp
// Add `friend` if the enum lies inside a `struct`
[friend] inline Style operator|(Style lhs, Style rhs) {
    return static_cast<Style>(static_cast<int>(lhs) | static_cast<int>(rhs));
}
```

### Relevant Files:

* API: [ChangeToEnumClass.hpp](some_unstable_lib/include/ChangeToEnumClass.hpp)
* User: [ChangeToEnumClassTest.cpp](tests/ChangeToEnumClassTest.cpp)


<!--

<a id="move_symb_to_ns"></a>

## Move types/symbols to a different namespace

### Initial code:

```cpp
namespace path::to::v1 {
  struct DontMove {};
  struct Bar {};
  constexpr int VAL = 42;
  enum SomeEnum { A, B, C };
}
```

### Scenario:

### Solution:

```cpp
// simply move them
namespace path::to::v2 {
struct Bar {
};
constexpr int VAL = 42;
enum SomeEnum { A, B, C };
}

namespace path::to::v1 {
struct DontMove {
};

// add using declarations for each
// moved symbol in the old namespace
using path::to::v2::Bar;
using path::to::v2::VAL;
using path::to::v2::SomeEnum;
// to not break "v2::A" uses
using path::to::v2::SomeEnum::A;
using path::to::v2::SomeEnum::B;
using path::to::v2::SomeEnum::C;
}
```

### Remarks:

### Relevant Files:


<a id="move_symb_to_class"></a>

## Move types/symbols to a different class

### Initial code:

```cpp
// Change: move these to NewClass
struct OldClass {
struct Bar {
};
constexpr int VAL = 42;
enum SomeEnum { A, B, C };
bool Foo() { return true; }
}
```

### Solution:

```cpp
// simply move them
struct NewClass {
struct Bar {
};
static constexpr int VAL = 42;
enum SomeEnum { A, B, C };
bool Foo() { return true; }
}

// In short, the old class should depend on the new one so you can add these:
struct OldClass {
using Bar = NewClass::Bar;
static constexpr int VAL = NewClass::VAL;
using SomeEnum = NewClass::SomeEnum;
// plain old enums are special, you also need to not break "OldClass::A" uses
static constexpr SomeEnum A = NewClass::A;
static constexpr SomeEnum B = NewClass::B;
static constexpr SomeEnum C = NewClass::C;

bool Foo() { return m_newCls.Foo(); }
private:
NewClass m_newCls;
}
```

### Remarks:

You need to add a `static constexpr` for each enum field since it
is unscoped in the old class.
-->


<a id="reasonably_safe_changes"></a>

## Other reasonably safe changes:

* Adding `const` to a member function (`T Get();` -> `T Get() const;`)
* Making a member function `static` (`T Get() const;` -> `static T Get();`)
	* the code `obj.Get(..)` will still compile.
* Adding `[[nodiscard]]` (`[[nodiscard]] int Get()` or `class [[nodiscard]] Result`)
	* should not be added to any random class or methods that have side effects (the user might have called the method for its side effectes)
* Adding `explicit` to a constructor with only 1 parameter.
	* except for classes that are expected to be implicitly constructed from that 1 parameter.
* Removing the `const&` when passing a copyable parameter (`void Set(const T&);` -> `void Set(T);`)
	* e.g. cannot remove `const&` from `void Set(const std::unique_ptr<T>&);` since unique_ptr is not copyable
* Removing the `const` when returning by value: `const RetT Get()`
	* except from non-private virtual methods, since the user's derived class might overide them.
	* this change can break some rare cases (see [neg-tests/RemoveConstReturnByValueTest.cpp](neg-tests/RemoveConstReturnByValueTest.cpp)):
		* `auto& val = Get();` was valid code until we removed the `const`
		* the return type was passed directly to a method with 2 overloads `Foo(const RetT&)` to `Foo(RetT&)`, it now calls the non-const one.
		* if `Get` returns `Base` now by value and `Derived` has an implicit constructor from `Base`,
		you can get incompatible types in ternary operators: `cond ? const Derived : Base` (`cond ? Derived{} : Get();`)
* Converting a class that has only static methods to a namespace
	* make sure the constructors and operator= are private, otherwise deprecate them before changing the class to a namespace
* Changing the underlying type of an enum (e.g. from `enum Flags` to `enum Flags: uint64_t`,
which happens when the enum is used as bit flags and we need to add another entry after `1<<31`)
	* except if users depend on sizeof(Flags), e.g. if they serialize it to binary data
	* `int x = Flags::X`, where `X=1<<31`, will still compile and it will not overflow,
	  even if `Flags` is now of type `uint64_T`, since the compiler sees that the value of `X` still fits inside `int`
	* users will get warnings if they compile with `-Wconversion` and assign
	  a value of type `Flags` into a now narrower type like an `int`


## TODOs

- Add tests to ensure no breakings for dynamic libraries. (So we check ODR violations as well)
- Change a base class by making the new one extend from the old one
- ...
