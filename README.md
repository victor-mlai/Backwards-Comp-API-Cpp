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

/include contains the "unstable" library which contains both the old and the
new API, depending on OLD_CODE_ENABLED.

The macro OLD_CODE_ENABLED controls whether the code before or after the API change.

/tests represents the users of the library, and all their code must compile
after the API change. (Should not depend on the macro OLD_CODE_ENABLED)

/neg-tests contains code that should not compile after the API change.
(Should not depend on the macro OLD_CODE_ENABLED)

# Scenarios

Most of the following changes will break:
* function pointer aliases
* virtual methods in user's class that overrides the lib's base class methods.

So they will not be mentioned.

* Renames:
  [Rename a namespace](#mv-namespace)
* Misc:
  [Change Method Default Parameters](#change_defaults)




## <a name="mv-namespace"/> <span style="color:yellow">Rename a namespace</span>

**Scenario:** We need to update the namespace name to be more descriptive, more generic, or more specific.

**Initial code:**
```cpp
namespace path::to::v1 { ... }
```

**Request:** Change from `path::to::v1` to `path::to::v2`

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

**Remarks:** `[[deprecated]]` attribute doesn't work on namespace aliases.
You can try compiler specific directives (Eg. `#pragma deprecated(keyword)` on msvc)

The alias should preferably be at the top of the file (this is why the empty namespace `namespace path::to::v2 {}` was added)





## <a name="change_defaults"/> <span style="color:yellow">Change method taking default parameters to taking struct</span>

**Scenario:** Your method receives too many default parameters now and it gets
harder for users to call it with only 1 or 2 parameters changed. We still need
to support users calling the method with only the mandatory arguments without
making it "ambiguous" for the compiler.

**Initial code:**
```cpp
void SomeMethod(
    const int mandatory,
    const bool opt1 = false,
    const float opt2 = 1e-6
) {...}
```

**Request:** Change the method to receive a struct containing these parameters.

**Solution:** If you just add the new `SomeMethod`, calling `SomeMethod` with
just the mandatory parameters becomes ambiguous, the compiler won't
know which of the 2 methods to choose. To tell it to prefer the newer
one we need to make the old one less specialized by making it a template.

```diff
+ template<int = 0>
= void SomeMethod(
=     const int mandatory,
=     const bool opt1 = false,
=     const float opt2 = 1e-6
<=> ) { ... }
+ ) {
+  // Calls the new implementation now
+  SomeMethod(mandatory, SomeMethodOpts{opt1, opt2});
+ }
+ 
+ struct SomeMethodOpts { bool opt1 = false; float opt2 = 1e-6; };
+ void SomeMethod(
+     const int mandatory,
+     SomeMethodOpts opts = {}
<=> ) { ... }
```

**Remarks:** You can deprecate the old `SomeMethod`




## <a name="change_defaults"/> <span style="color:yellow">Rename a type</span>

**Scenario:** We need to update the struct name to be more descriptive, more generic, or more specific.

**Initial code:**
```cpp
struct OldName { ... };
```

**Request:** Change the name to `NewName`.

**Solution:** We can use a type alias.

```diff
- struct OldName { ... };
+ struct NewName { ... };
+ using OldName = NewName;
```

**Remarks:**
* You can deprecate the old `OldName`.
* Your users might learn the hard way that they shouldn't forward declare foreign types.





## <a name="change_defaults"/> <span style="color:yellow">Move a header</span>

**Scenario:** We need to move/rename a header.

**Initial code:**
```
// v1/OldName.hpp
...
```

**Request:** Change the path to `v2/NewName.hpp`.

**Solution:** Make the old file include the new one.

```
// v2/NewName.hpp
...

// v1/OldName.hpp
#pragma once
#include "v2/NewName.hpp"
```

**Remarks:**
* You can deprecate the old file by inserting a "compilation error":
`#error Deprecated header, please include "v2/NewName.hpp" instead.`




## <a name="change_defaults"/> <span style="color:yellow">Changing the return type (or "overloading" by return type)</span>

**Scenario:** 

**Initial code:**
```cpp
OldRetT SomeMethod();
```

**Request:** return `NewRetT` instead

**Solution:** Add a new class with an implicit cast operator to `NewRetT` and `OldRetT`.

```diff
+ struct SomeMethodRetT {
+   template <int = 0> // If the conversion is ambigous, templating makes this method a worser overload candidate.
+   operator OldRetT () const { ... }
+   operator NewRetT () const { ... }
+ };
- OldRetT SomeMethod();
+ SomeMethodRetT SomeMethod();
```

**Remarks:** The implicit cast may happen in unintended scenarios.
Also, you might want to deprecate the `OldRetT` cast operator.

**Errors:**
If `OldRetT` was a `const T&` and `NewRetT` is just `T` and if the user had
a method that was just forwarding the returned value, now they will get a
compiler error caused by trying to return a reference to a temporary object.
See [ReturnTypeChange](include/ReturnTypeChange.hpp)




## <a name="change_defaults"/> <span style="color:yellow">TODO: Move types/symbols to a different namespace</span>

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



## <a name="change_defaults"/> <span style="color:yellow">TODO: Move types/symbols to a different class</span>

```cpp
// Todo: move these to NewClass
struct OldClass {
  struct Bar {};
  constexpr int VAL = 42;
  enum SomeEnum { A, B, C };
}
```


```cpp
// simply move them
struct NewClass {
  struct Bar {};
  static constexpr int VAL = 42;
  enum SomeEnum { A, B, C };
}

// then add these in the old class
struct OldClass {
  using Bar = NewClass::Bar;
  static constexpr int VAL = NewClass::VAL;
  using SomeEnum = NewClass::SomeEnum;
  // plain old enums are special, you also need to not break "OldClass::A" uses
  static constexpr SomeEnum A = NewClass::A;
  static constexpr SomeEnum B = NewClass::B;
  static constexpr SomeEnum C = NewClass::C;
}
```

**Remarks:** You need to add a `static constexpr` for each enum field since it is unscoped in the old class.






## Other reasonably safe changes:
* removing the `const` when returning by value: `const RetT some_func()`
* transforming a class with only static methods to a namespace
* changing the underlying type of an enum
* TODO: removing the `&` when returning by `const &`










## Quirks

<table>
<tr>
  <td> Change Description </td>
  <td> Initial code + change </td>
  <td> Solution </td>
  <td> Drawbacks </td>
</tr>


<tr>
  <td> Change the underlying type of an unscoped enum </td>

  <td>

When working with bitfields,
we might end up in the situation where we
want to add D > C however we are at the limit.

```cpp
// Todo: change to std::uint64_t
enum SomeEnum {
  ...,
  C = 0xFFFF'FFFF
};
```

  </td>

  <td>

```cpp
enum SomeEnum : std::uint64_t {
  ...,
  C = 0xFFFF'FFFF,
  D = 0x1'0000'0000
};
```

This is already backwards-compatible, because
the following code does not give any underflow warnings
(which would have been a breaking change
when compiled with "Treat Warnings as Errors").

```cpp
unsigned x = SomeEnum::C;
```

  </td> 
  <td>

Strangely, none.

  </td> 

</tr>

<tr>
  <td> Add Y::fun(Y::X) </td>

  <td>

```cpp
namespace Y
{
 struct X {..};
 // TODO: add fun
}

...

// user already has fun
// in their namespace
namespace User
{
  void fun(Y::X);
}

// Their code:
namespace User
{
  void SomeFun() {
    Y::X x;
    fun(x); // calls User::fun
  }
}
```

  </td>

  <td>

```cpp
namespace Y
{
 struct X {..};
 // adding this changed
 // user's code
 void fun(Y::X);
}
...
// Their code:
namespace User
{
  void SomeFun() {
    Y::X x;
    fun(x); // now calls Y::fun
  }
}
```

  </td> 
  <td>

Due to ADL (Argument Dependant Lookup, aka if your type X is in namespace Y, when calling fun(X), fun will be looked up
by the compiler in the namespace Y first.), if you add a method in your namespace, the user might now call this method
instead of theirs.

  </td>

</tr>


</table>

# Todo

- Add tests to ensure no breakings for dynamic libraries. (Hint: ODR + static members)
- Add items to list:
    - Change enum to enum class
    - Add parameters to a function
    - Change a base class
    - ...
