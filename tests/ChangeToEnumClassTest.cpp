#include "ChangeToEnumClass.hpp"
#include "gtest/gtest.h"

TEST(ChangeToEnumClass, Basic) {
    constexpr Logger logger;
    // asserts the correct overload was called
    static_assert(logger.Log(2) == Logger::LogInt);
    static_assert(logger.Log(Logger::Handler::StdOut) == Logger::LogEnum);
    static_assert(logger.Log(Logger::StdOut | Logger::StdErr) == Logger::LogInt);
}
