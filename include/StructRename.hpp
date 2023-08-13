#pragma once

#ifdef OLD_CODE_ENABLED
// Change: rename OldName to NewName
struct OldName {};
#else
// simply rename
struct NewName {};

// then add this type alias
using OldName = NewName;
#endif
