# Backwards Compatibility tricks for API changes in C++

This project contains a list of all the changes you would
like to do to your public API together with tricks on making
these changes as non-breaking/backwards compatible,
and as boilerplate free as possible.


# Contributing

This project just got created.

Please feel free to create Issues and Pull Requests
in case you find this list incomplete.


## Project structure

include/ represents the "unstable" library which contains both the old and the
new API. The macro BC_API_CHANGED controls whether the code is before or after the API change.

tests/ represents the users of the library whose code must compile
after the API change. Please follow this format: `<FileNameTest.cpp>`

neg-tests/ contains code that will not compile after the API change.
One API change can lead to multiple types of compile errors.
Each of these errors must be isolated in a single file.
Please follow this format: `<FileName_ErrorExpected.cpp>`


# !! Notice

Most of the changes in this list will break:
* ABI
* function pointer aliases
* forward declarations

Some of these changes should not be done to:
* virtual methods (Eg. removing `const` when returning by value will break user's code if they override this method)

(I assume these are not a concern for your library)


# Summary

* Renames:
  * [Rename a namespace](#mv-namespace)
  * [Rename a type](#mv-type)
  * [Move a header](#mv-header)
* Misc:
  * [Change Method Default Parameters](#change_defaults)
  * [Changing the return type (or "overloading" by return type)](#change_ret_type)
  * [TODO: Move types/symbols to a different namespace](#move_symb_to_ns)
  * [TODO: Move types/symbols to a different class](#move_symb_to_class)
* [Reasonably safe changes](#reasonably_safe_changes)
* [Quirks](#quirks)
* [ToDo]

# List

<details>
<summary id="mv-namespace">Rename a namespace</summary>

**Initial code:**
```cpp
namespace path::to::v1 { ... }
```

**Scenario:** We maybe need to change the namespace name to be more descriptive,
or more generic, or more specific.
We will change it from `path::to::v1` to `path::to::v2`.

**Solution:** Rename the old namespace to the new one and add a namespace alias for the old one.

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
You can try compiler specific directives (Eg. `#pragma deprecated(keyword)` on msvc)

* The alias should preferably be first at the top of the file
(this is why the empty namespace `namespace path::to::v2 {}` was added)


**Files:**
* API: [NamespaceRename.hpp](include/NamespaceRename.hpp)
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

**Scenario:** This method receives too many default parameters, and it only becomes
harder for users to call it with only 1 or 2 parameters changed. We need to change
the method to receive a struct containing these parameters instead.

**Solution:** If you just add the new `SomeMethod`, users calling `SomeMethod` with
just the mandatory parameters will have the compiler complain about ambiguity (it won't
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
* API: [MethodDefaultParams.hpp](include/MethodDefaultParams.hpp)
* User: [MethodDefaultParamsTest.cpp](tests/MethodDefaultParamsTest.cpp)

</details>


<details>
  <summary id="mv-type">Rename a type</summary>

**Initial code:**
```cpp
struct OldName { ... };
```

**Scenario:** We need to update the struct name to be more descriptive,
more generic, or more specific. We will change it to `NewName`.

**Solution:** We can use a type alias.

```diff
- struct OldName { ... };
+ struct NewName { ... };
+ using OldName = NewName;
```

**Remarks:**
* You can deprecate the old `OldName`.
* The users might learn the hard way that they shouldn't forward declare foreign types.

**Files:**
* API: [StructRename.hpp](include/StructRename.hpp)
* User: [StructRenameTest.cpp](tests/StructRenameTest.cpp)

</details>


<details>
  <summary id="mv-header">Move a header</summary>

**Initial code:**

 v1/OldName.hpp:
```cpp
// the code
```

**Scenario:** We need to move/rename the header to `v2/NewName.hpp`.

**Solution:** Make the old file include the new one.

old file moved/renamed to v2/NewName.hpp:
```cpp
// the code
```

v1/OldName.hpp:
```cpp
#include "v2/NewName.hpp"

// You can also deprecate it by inserting a "compilation warning":
// #warning OldName.hpp is deprecated, please include "v2/NewName.hpp" instead.`
```

**Remarks:** Rename/move using the versioning tool (Git/SVN) so you don't lose blame history.

**Files:** -

</details>


<details>
  <summary id="change_ret_type">Changing the return type (or "overloading" by return type)</summary>

**Initial code:**
```cpp
// (1) change primitive `T` to `NewUserDefT`
bool TryParseString(std::string);

// (2) change primitive `const T&` to primitive `T`
struct Strukt {
    const float& GetMemF() const { return m_memF; }
};
```

**Scenario:**

(1) `TryParseString` method returns true if it succeeds, otherwise false.
Make this method return some error message as well so the user knows why it failed (why it returned false).

(2) `Strukt::GetMemF` returns primitive types as const& which is bad for multiple reasons. We need to return by value.

However, we cannot just overload a function by return type and then deprecate it.

**Solution:**

(1) return a new type that can be implicitly converted to bool.

(2) Add a new class with an implicit cast operator to `NewRetT` and `OldRetT`.
  - (2.1) Additionally, if the compiler can't decide between the 2 cast operators at overload resolution,
templating the old one makes it choose the new overload candidate since it's more specialized.
  - (2.2) Return GetterRetT by const& to avoid dangling references in user's Wrappers that only 
forward the old `const float&`

```diff
// (1) change primitive `T` to `NewUserDefT`
+ struct TryParseStringResult { // mimics std::expected<void, std::string>
+     explicit operator bool() const { return !m_errMsg.has_value(); }
+     const std::string& error() const { return m_errMsg.value(); }
+ private:
+     std::optional<std::string> m_errMsg;
+ };
- bool TryParseString(std::string);
+ TryParseStringResult TryParseString(std::string);

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
Also, you might want to deprecate the `OldRetT` cast operator and the `GetterRetT` type.


**Files:**
* API: [ReturnTypeChange.hpp](include/ReturnTypeChange.hpp)
* User: [ReturnTypeChangeTest.cpp](tests/ReturnTypeChangeTest.cpp)
* Neg: *ToDo* add unintended uses in [ReturnTypeChange_Auto.cpp](neg-tests/ReturnTypeChange_Auto.cpp)

</details>


## <a name="move_symb_to_ns"/> TODO: Move types/symbols to a different namespace

```cpp
// Todo: move some to v2
namespace path::to::v1 {
  struct NotMoved {};
  struct Bar {};
  constexpr int VAL = 42;
  enum SomeEnum { A, B, C };
}
```

```cpp
// simply move them
namespace path::to::v2 {
  struct Bar {};
  constexpr int VAL = 42;
  enum SomeEnum { A, B, C };
}

namespace path::to::v1 {
  struct NotMoved {};
  
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



## <a name="move_symb_to_class"/> TODO: Move types/symbols to a different class

```cpp
// Change: move these to NewClass
struct OldClass {
  struct Bar {};
  constexpr int VAL = 42;
  enum SomeEnum { A, B, C };
  bool Foo() { return true; }
}
```


```cpp
// simply move them
struct NewClass {
  struct Bar {};
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

**Remarks:** You need to add a `static constexpr` for each enum field since it is unscoped in the old class.



## <a name="reasonably_safe_changes"/> Reasonably safe changes:
* removing the `const` when returning by value: `const RetT some_func()`
* convert a class with only static methods to a namespace
* if your non-static method doesn't use `this`, convert it to a static one (`obj.SomeMethod(..)` will still work)
* changing the underlying type of an enum



## <a name="quirks"/> Quirks

### "Widening" the underlying type of an unscoped enum

**Todo** add tests

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

### Library breaks user code by adding a method in their own namespace

**Todo** showcase with a negative test

**Scenario:** Library wants to add a new method in their own namespace but
which takes as input an object from their own namespace.

```cpp
// Lib/SomeHeader.hpp
namespace Lib
{
    struct X {..};
}

// UserCode/Core.hpp
// Coincidentally, user already has a Foo in their namespace
namespace User
{
    void Foo(const Y::X&);
}

// UserCode/Core.cpp
namespace User
{
    void SomeFun() {
        Lib::X x;
        Foo(x); // calls User::Foo
    }
}
```

```diff
// Lib/SomeHeader.hpp
namespace Lib
{
    struct X {..};

+    void Foo(const X&);
}

// UserCode/Core.cpp
namespace User
{
    void SomeFun() {
        Lib::X x;
-        Foo(x); // calls User::Foo
+        Foo(x); // now calls Lib::Foo
    }
}
```

Due to ADL (Argument Dependant Lookup - if your type X is in namespace Lib, when calling Foo(X), Foo will be looked up
by the compiler in the namespace Lib first), if you add a method in your namespace, the user might now call this method
instead of theirs.


# Todo

- Add tests to ensure no breakings for dynamic libraries. (Hint: ODR + static members)
- Add items to list:
    - Change enum to enum class
    - Add parameters to a function
    - Change a base class
    - ...
