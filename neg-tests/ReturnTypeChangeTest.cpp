#include "ReturnTypeChange.hpp"

int main()
{
    using FailingMethodFuncPtrT = bool (*)(bool);
    FailingMethodFuncPtrT funcPtr = &FailingMethod;
    (void)funcPtr;
}
