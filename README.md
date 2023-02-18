# Backwards Compatibility tricks for API changes in C++

This project contains a list of all the changes you would
like to do to your public API toghether with tricks on making
these changes as non-breaking/backwards compatible,
and as boilerplate free as possible.

# Contributing

This project just got created.

Please feel free to create Issues and Pull Requests
in case you find this list incomplete.

# List

## Changes
-------------------------------------------------------------------------

<table>
<tr>
  <td> Change Description </td>
  <td> Initial code + change </td>
  <td> Solution </td>
  <td> Drawbacks </td>
</tr>



<tr>
  <td> Change function return type </td>

  <td>

```cpp
// Todo: return UserDefT instead
bool IsOdd();
```

  </td>

  <td>

```cpp
UserDefT IsOdd();

// then add an implicit cast
// operator inside UserDefT
struct UserDefT {
  operator bool() const {...}
};
```

  </td>
  <td>

Users will get errors if they tried to use IsOdd through a function pointer:
```cpp
using IsOddFuncT = bool (*)();
IsOddFuncT func = &IsOdd;
```

  </td>

</tr>

  
  
<tr>
  <td> Change parameter type </td>

  <td>

```cpp
// Todo: pass UserDefT instead
void IsOdd(bool);
```

  </td>

  <td>

```cpp
void IsOdd(UserDefT);

// add an implicit constructor
// inside UserDefT
struct UserDefT {
  UserDefT(bool) {...}
};
```

  </td>
  <td>

Users will get errors if they tried to use IsOdd through a function pointer:
```cpp
using IsOddFuncT = void (*)(bool);
IsOddFuncT func = &IsOdd;
```

The new constructor might unknowningly be used instead.
```cpp
// calls UserDefT(bool);
// instead of UserDefT(int);
UserDefT(1);
```

  </td>

</tr>



<tr>
  <td> Change default parameters </td>

  <td>

```cpp
// Todo: have Opts
// as default instead
void IsOdd(
  int nonDef,
  bool val = true,
  float tol = 0.01);
```

  </td>

  <td>

```cpp
void IsOdd(
  int nonDef,
  Opts opts = {});

// to avoid "ambiguous call"
// to function", make this
// function less "specialized"
template<int = 0>
void IsOdd(
  int nonDef,
  bool val = true,
  float tol = 0.01);
```

  </td>
  <td>

??? should still work
Users will get errors if they tried to use IsOdd through a function pointer:
```cpp
using IsOddFuncT =
  void (*)(int, bool, float);
IsOddFuncT func = &IsOdd;
```

The new constructor might unknowningly be used instead.
```cpp
// calls UserDefT(bool);
// instead of UserDefT(int);
UserDefT(1);
```

  </td>

</tr>

</table>


  
  
  
  
  
  
## Renames
-------------------------------------------------------------------------

<table>
<tr>
  <td> Change Description </td>
  <td> Initial code + change </td>
  <td> Solution </td>
  <td> Drawbacks </td>
</tr>



<tr>
  <td> Rename a class </td>

  <td>

```cpp
// Todo: rename to FinalName
struct InitName {};
```

  </td>

  <td>

```cpp
// simply rename
struct FinalName {};

// then add a type alias
using InitName = FinalName;
```

  </td>
  <td>

Users will get compile errors if they were forward declaring `InitName`.

  </td>

</tr>



<tr>
  <td> Rename a namespace </td>

  <td>

```cpp
// Todo: rename to v2
namespace path::to::v1 {
  struct Bar {};
  constexpr int VAL = 42;
  enum SomeEnum { A, B, C };
}
```

  </td>

  <td>

```cpp
// simply rename
namespace path::to::v2 {
  struct Bar {};
  constexpr int VAL = 42;
  enum SomeEnum { A, B, C };
}

// then add a namespace alias
namespace path::to {
  namespace v1 = path::to::v2;
}
```

  </td>

  <td>

Users will get compile errors if they were forward declaring anything from the old namespace.

  </td> 

</tr>


<tr>
  <td> Rename a file </td>

  <td>

```cpp
// Todo: rename to FinalName
InitName.hpp
```

  </td>

  <td>

```cpp
// simply rename
FinalName.hpp

// then, recreate InitName.hpp
// to include FinalName.hpp
#pragma once
#include "FinalName.hpp"
//optionally, to inform users:
#error Deprecated header,\
please include "FinalName.hpp" instead.
```

  </td>
  <td>

?

  </td>

</tr>


</table>



## Moves
-------------------------------------------------------------------------

<table>
<tr>
  <td> Change Description </td>
  <td> Initial code + change </td>
  <td> Solution </td>
  <td> Drawbacks </td>
</tr>

<tr>
  <td> Move a class/symbol to a different namespace </td>

  <td>

```cpp
// Todo: move some to v2
namespace path::to::v1 {
  struct NotMoved {};
  struct Bar {};
  constexpr int VAL = 42;
  enum SomeEnum { A, B, C };
}
```

  </td>

  <td>

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

  </td>

  <td>

You need to add a using declaration for each
enum field since it is unscoped in the old namespace.

  </td> 

</tr>


<tr>
  <td>

Move some internal symbols to a different class

  </td>

  <td>

```cpp
// Todo: move these to NewClass
struct OldClass {
  struct Bar {};
  constexpr int VAL = 42;
  enum SomeEnum { A, B, C };
}
```

  </td>

  <td>

```cpp
// simply move them
struct NewClass {
  struct Bar {};
  static constexpr int VAL = 42;
  enum SomeEnum { A, B, C };
}

// then add these in the old class
struct OldClass {
  using Bar = OldClass::Bar;
  static constexpr int VAL = OldClass::VAL;
  using SomeEnum = OldClass::SomeEnum;
  // to not break "OldClass::A" uses
  static constexpr SomeEnum A = OldClass::A;
  static constexpr SomeEnum B = OldClass::B;
  static constexpr SomeEnum C = OldClass::C;
}
```

  </td>

  <td>

You need to add a `static constexpr` for each
enum field since it is unscoped in the old class.

  </td> 

</tr>


</table>




## Quirks
-------------------------------------------------------------------------

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

Due to ADL (Argument Dependant Lookup, aka if your type X is in namespace Y, when calling fun(X), fun will be looked up by the compiler in the namespace Y first.), if you add a method in your namespace, the user might now call this method instead of theirs.

  </td>

</tr>


</table>

# Todo

- Add tests to ensure no breakings on user side
- Add items to list:
    - Change enum to enum class
    - Add parameters to a function
    - Change a base class
    - ...
