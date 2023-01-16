# Backwards-Comp-API-Cpp
This project contains a list of all the changes you would like to do to your public API toghether with ways on making these changes as non-breaking/backwards compatible, and as boilerplate free as possible.

# Contributing
This project just got created.

Please feel free to create Issues and Pull Requests in case you find this list incomplete.

# List

## Renames
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
// rename InitName to FinalName
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
// rename v1 to v2
namespace path::to::v1 {
  struct Bar {};
  constexpr Type VAL = 42;
  enum SomeEnum { A, B, C };
}
```

  </td>
  
  <td>

```cpp
// simply rename
namespace path::to::v2 {
  struct Bar {};
  constexpr Type VAL = 42;
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




## Misc

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
enum SomeEnum {
  ...,
  C = 0xFFFF'FFFF
};
```

  </td>
  
  <td>
  
This is already backwards-compatible.

No further changes needed.

```cpp
enum SomeEnum : std::uint64_t {
  ...,
  C = 0xFFFF'FFFF,
  D = 0x1'0000'0000
};
```

  </td> 
  <td>
  
  Users won't be able to forward declare `InitName` anymore.
  
  </td> 

</tr>


</table>

# Todo
- Change enum to enum class
- Add parameters
- Change base class
- ...
