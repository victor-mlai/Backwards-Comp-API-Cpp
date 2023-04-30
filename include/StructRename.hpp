#pragma once

#ifdef OLD_CODE_ENABLED
// Todo: rename InitName to FinalName
struct InitName {};
#else
// simply rename
struct FinalName {};

// then add this type alias
using InitName = FinalName;
#endif
