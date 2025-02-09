# Backwards Compatibility tricks for API changes in C++

This project contains "tricks" on how to make API changes backwards compatible
(Eg. changing return types, renaming namespaces, changing enums to enum classes etc.)

It contains also tests (see /tests and /neg-tests) to prove that
the tricks make them backwards compatible and don't break something else.

Some of them also have negative tests to show some edge cases for when the
API change trick breaks code that previously compiled.

# Notice

This project just got created.

Please feel free to create Issues and Pull Requests to improve this list.

These tricks are not focussed to not **break ABI**,
forward declarations or function pointer aliases.

# Project internals

/some_unstable_lib contains a header file for each API change in the list further down.
Each header either exposes the old API or the changed one based on the macro BC_API_CHANGED.

/tests represents the users of the library whose code must compile
before and after the API change.

/neg-tests contains code that should not compile after the API change.
One file per each such case.

All tests are run twice, with the macro BC_API_CHANGED OFF (before the API change)
and ON (after).


# Overview
* [Rename/Move a namespace](#mv-namespace)
* [Change method with default parameters to receive a struct with those parameters](#change_defaults)
* [Rename/Move a type](#mv-type)
* [Rename/Move a header](#mv-header)
* [Change the return type (or "overloading" by return type)](#change_ret_type)
* [Reasonably safe changes](#reasonably_safe_changes)
<!--
* [Move symbols to a different namespace](#move_symb_to_ns)
* [Move symbols to a different class](#move_symb_to_class)
-->


# How to, in a backwards compatible way:

<details>
  <summary id="mv-namespace">Rename a namespace</summary>

**Initial code:**

```cpp
namespace path::to::v1 { ... }
```

**Scenario:** We maybe need to change the namespace name to fix a typo.
We will change it from `path::to::v1` to `path::to::v2`.

**Solution:** Rename the old namespace to the new one and add a namespace alias
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

**Remarks:**

* `[[deprecated]]` attribute doesn't work on namespace aliases.
  You can try compiler specific directives (Eg. `#pragma deprecated(keyword)` for
  msvc)

* The empty namespace `namespace path::to::v2 {}` was added at the top of the
file for visibility purposes

**Files:**

* API: [NamespaceRename.hpp](some_unstable_lib/include/NamespaceRename.hpp)
* User: [NamespaceRenameTest.cpp](tests/NamespaceRenameTest.cpp)

</details>


<details>
  <summary id="change_defaults">Change method taking default parameters to taking struct</summary>

**Initial code:**

```cpp
void SomeMethod(
    const int mandatory,
    const bool opt1 = false,
    const float opt2 = 1e-6
) { ... }
```

**Scenario:** This method receives too many default parameters, and it only
becomes
harder for users to call it with only 1 or 2 parameters changed. We need to
change
the method to receive a struct containing these parameters instead.

**Solution:** If you just add the new `SomeMethod`, users calling `SomeMethod`
with
just the mandatory parameters will have the compiler complain about ambiguity (
it won't
know which of the 2 methods to choose from). To tell it to prefer the newer
one we need to make the old one less specialized by making it a template.

```diff
+ template<int = 0>
void SomeMethod(
    const int mandatory,
    const bool opt1 = false,
    const float opt2 = 1e-6
+ ) {
+  // Call the new implementation now
+  SomeMethod(mandatory, SomeMethodOpts{opt1, opt2});
+ }
+ 
+ struct SomeMethodOpts { bool opt1 = false; float opt2 = 1e-6; };
+ void SomeMethod(
+     const int mandatory,
+     SomeMethodOpts opts = {}
) { ... }
```

**Remarks:** You can deprecate the old `SomeMethod` (now a template)

**Files:**

* API: [MethodDefaultParams.hpp](some_unstable_lib/include/MethodDefaultParams.hpp)
* User: [MethodDefaultParamsTest.cpp](tests/MethodDefaultParamsTest.cpp)

</details>


<details>
  <summary id="mv-type">Rename a type</summary>

**Initial code:**

```cpp
struct OldName { ... };
```

**Scenario:** We maybe need to update the struct name to fix a typo.
We will change it to `NewName`.

**Solution:** We can use a type alias.

```diff
- struct OldName { ... };
+ struct NewName { ... };
+ using OldName = NewName;
```

**Remarks:**

* You can deprecate the old `OldName`.
* The users might learn the hard way that they shouldn't forward declare foreign
  types.

**Files:**

* API: [StructRename.hpp](some_unstable_lib/include/StructRename.hpp)
* User: [StructRenameTest.cpp](tests/StructRenameTest.cpp)

</details>


<details>
  <summary id="mv-header">Move a header</summary>

**Initial code:**

```cpp
// v1/OldName.hpp:
...
```

**Scenario:** We need to move/rename the header to `v2/NewName.hpp`.

**Solution:**

1. Move/rename the header:

```diff
- // v1/OldName.hpp:
+ // v2/NewName.hpp: <- only moved/renamed
...
```

2. Create a compatibility header file in the old location that includes
  the renamed/moved one.

```cpp
// v1/OldName.hpp: <- created to only include the renamed header + deprecation notice
#include "v2/NewName.hpp"

// You can also deprecate it by inserting a compilation error/warning:
// #error/warning OldName.hpp is deprecated, include "v2/NewName.hpp".`
```

**Remarks:** Rename/move using the versioning tool (Git/SVN) so you don't lose
blame history.

</details>


<details>
  <summary id="change_ret_type">Change the return type (or "overload" by return type)</summary>

**Initial code:**

```cpp
// (1) change some primitive `T` to `NewUserDefT`
bool CheckPassword(std::string);

// (2) change some primitive `const T&` to primitive `T`
struct Strukt {
const float& GetMemF() const {
return m_memF; }
};
```

**Scenario:**

(1) `CheckPassword` method returns true if it succeeds, otherwise false.
Make this method return some meaningful error message so the user knows why it
failed (why it returned false).

(2) `Strukt::GetMemF` returns primitive types as const& which is bad for
multiple reasons. We need to return by value.

However, we cannot just overload a function by return type and then deprecate
it.

**Solution:**

For (1): Return a new type that can be implicitly casted to bool.

- (1.1): If you don't want it to be implicitly casted to other primitive types like
  `int`, since C++20 you can make it conditionally explicit.
  (In the tests, `int x = CheckPassword("");` doesn't compile after the API change,
  while `bool x = CheckPassword("");` does)

For (2): Add a new class with an implicit cast operator to `NewRetT` and `OldRetT`.

- (2.1) Additionally, if the compiler can't decide between the 2 cast operators
  at overload resolution,
  templating the old one makes it choose the new overload candidate since it's
  more specialized.
- (2.2) Return GetterRetT by const& to avoid dangling references in user's
  Wrappers that only
  forward the old `const float&`

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

// (2) change primitive `const T&` to primitive `T`
+ struct SomeMethodRetT {
+   template <int = 0> // (2.1)
+   operator OldRetT () const { ... }
+   operator NewRetT () const { ... }
+ };
struct Strukt {
-   const float& GetMemF() const { return m_memF; }
-   float m_memF = 3.f;
+   // (2.2)
+   const GetterRetT& GetMemF() const { return m_memF; }
+   GetterRetT m_memF = 3.f;
};
```

**Remarks:** The implicit cast may happen in unintended scenarios.
Also, you might want to deprecate the `OldRetT` cast operator and
the `GetterRetT` type.

**Files:**

* API: [include/ReturnTypeChange.hpp](some_unstable_lib/include/ReturnTypeChange.hpp)
  [include/ReturnTypeChangeByValue.hpp](some_unstable_lib/include/ReturnTypeChangeByValue.hpp)
* User: [tests/ReturnTypeChangeTest.cpp](tests/ReturnTypeChangeTest.cpp)
  [tests/ReturnTypeChangeByValueTest.hpp](tests/ReturnTypeChangeByValueTest.hpp)
* Neg: [neg-tests/ReturnTypeChangeTest.cpp](neg-tests/ReturnTypeChangeTest.cpp)

</details>


<details>
  <summary id="change_to_enum_class">Change old-style enum to enum class</summary>

**Initial code:**

```cpp
enum Handler {
    StdOut,
    StdErr,
    File,
};
```

**Scenario:**

CTO: > Upgrading old-style enums to enum classes shouldn't be too hard ... Right?

**Solution:**

In order to not break unscoped uses of the enum, we should define static
variables for each enum entry.

```diff
- enum Handler {
+ enum class Handler {
    StdOut,
    StdErr,
    File,
};

+ static constexpr Handler StdOut = Handler::StdOut;
+ static constexpr Handler StdErr = Handler::StdErr;
+ static constexpr Handler File = Handler::File;
```

**Remarks:** 

If the enum was used as bit flags, define bitwise operators as well.

Note: Assuming you have a `Log` method with 2 overloads, for `int` and for `Handler`,
and expect `Log(StdOut | StdErr)` to still call `Log(int)`, then
the return type for the bitwise operators should be `int`, otherwise `Handler`:

```cpp
// Add `friend` if the enum lies inside a `struct`
[friend] constexpr int operator|(Handler lhs, Handler rhs) {
    return static_cast<int>(lhs) | static_cast<int>(rhs);
}
```

**Files:**

* API: [ChangeToEnumClass.hpp](some_unstable_lib/include/ChangeToEnumClass.hpp)
* User: [ChangeToEnumClassTest.cpp](tests/ChangeToEnumClassTest.cpp)

</details>

<!--
<details>
  <summary id="move_symb_to_ns">Move types/symbols to a different namespace</summary>

**Initial code:**

```cpp
namespace path::to::v1 {
  struct DontMove {};
  struct Bar {};
  constexpr int VAL = 42;
  enum SomeEnum { A, B, C };
}
```

**Scenario:**

**Solution:**

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

**Remarks:**

**Files:**

</details>


<details>
  <summary id="move_symb_to_class">Move types/symbols to a different class</summary>

**Initial code:**

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

**Remarks:** You need to add a `static constexpr` for each enum field since it
is unscoped in the old class.

</details>
-->

## Other reasonably safe changes:

TODO: add unit tests for these:

* removing the `const` when returning by value: `const RetT some_func()`
  (except when it is a virtual method, since it can be overriden by users)
* changing a class that has only static methods to a namespace
* add `static` to member function (`obj.SomeMethod(..)` still works)
* changing the underlying type of an enum
* adding `[[nodiscard]]` or `explicit` (since they prevent bugs)

<!--
## <a name="quirks"/> Quirks

<details>
  <summary id="widen_enum">"Widening" the underlying type of an unscoped enum</summary>

**!Todo** add tests for this claim

**Scenario:** When using enums to work with bitfields,
we might not think ahead and end up in a situation where we
want to add a new enum value D after C however we are at the
limit of the default underlying type, i.e. `int`.

```cpp
enum SomeEnum {
  ...,
  B = 1 << 30,
  C = 1 << 31,
  // D = 1 << 32, UB or a warning/error when uncommented
};
```

Change to

```cpp
enum SomeEnum : std::uint64_t {
  ...,
  B = 1 << 30,
  C = 1 << 31,
  D = 1ull << 32,
};
```

**Remarks:**
This is already backwards-compatible, because the following code does not
give any underflow warnings.

```cpp
unsigned x = SomeEnum::C;
```

</details>
-->

## Other TODOs

- Add tests to ensure no breakings for dynamic libraries. (Hint: ODR + static
  members)
- Change a base class by making the new one extend from the old one
- ...
