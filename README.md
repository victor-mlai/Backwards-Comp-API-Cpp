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
// Todo: rename InitName to FinalName
struct InitName {};
```

  </td>
  
  <td>

```cpp
// simply rename
struct FinalName {};

// then add this type alias
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
// Todo: rename v1 to v2
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

// add a using namespace
//  in the old namespace
namespace path::to::v1 {
  using namespace path::to::v2;
}
```

  </td>
  
  <td>
  
  `using namespace` is often frowned upon.
  
  </td> 

</tr>


</table>



## Moves
-------------------------------------------------------------------------

<table>
<tr>
  <td> Move a class/symbol to a different namespace </td>

  <td>

```cpp
// Todo: move these only to v2
namespace path::to::v1 {
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

// add using declarations for each
// symbol in the old namespace
namespace path::to::v1 {
  using path::to::v2::Bar;
  using path::to::v2::VAL;
  using path::to::v2::SomeEnum;
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




## Misc
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


</table>

# Todo
- Add tests to ensure no breakings on user side
- Add items to list:
  - Change enum to enum class
  - Add parameters to a function
  - Change a base class
  - ...
