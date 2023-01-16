# Backwards-Comp-API-Cpp
This project contains a list of all the changes you would like to do to your public API toghether with ways on making these changes non-breaking/backwards compatible.

# Contributing
This project just got created.

Please feel free to create Issues and Pull Requests in case you find this list incomplete.

# List

## Misc

## Renames
<table>
<tr>
  <td> Change Description </td>
  <td> Initial code + change </td>
  <td> Solution </td>
  <td> Drawbacks </td>
</tr>


// -----------------------------------------------------------


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
// simply rename and then
struct FinalName {};

// add this type alias
using InitName = FinalName;
```

  </td> 
  <td>
  
  Users won't be able to forward declare `InitName` anymore.
  
  </td> 

</tr>


// -----------------------------------------------------------


<tr>
  <td> Rename a namespace </td>

  <td>

```cpp
// rename v1 to v2
namespace path::to::v1 {
  struct Bar {};
  constexpr Type VAL = 42;
}
```

  </td>
  
  <td>

```cpp
// simply rename
namespace path::to::v2 {
  struct Bar {};
  constexpr Type VAL = 42;
}

// add using declarations
//  in the old namespace
namespace path::to::v1 {
  using path::to::v2::Bar;
  using path::to::v2::VAL;
}
```

  </td>
  
  <td>
  
  ?
  
  </td> 

</tr>



</table>


