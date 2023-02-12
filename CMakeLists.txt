cmake_minimum_required(VERSION 3.14)

option(BCAPI_BUILD_OLD_VERSION "If checked, you will build the old version." ON)

project(bcAPI
        HOMEPAGE_URL "https://github.com/victorlaurentiu/Backwards-Comp-API-Cpp"
        DESCRIPTION "Tricks in C++ for making API changes as non-breaking and boilerplate free as possible"
        LANGUAGES CXX
        )

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG b796f7d44681514f58a683a3a71ff17c94edb0c1 # release-1.13.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_subdirectory(tests)

enable_testing()
add_test(NAME MyTests COMMAND bcAPI_tests)