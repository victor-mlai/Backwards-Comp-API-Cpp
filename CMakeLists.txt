cmake_minimum_required(VERSION 3.14)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(BCAPI_BUILD_OLD_VERSION "If checked, you will build the old version." OFF)

project(
    bcAPI
    HOMEPAGE_URL "https://github.com/victorlaurentiu/Backwards-Comp-API-Cpp"
    DESCRIPTION "Tricks in C++ for making API changes as non-breaking and boilerplate free as possible"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(include)
enable_testing()
add_subdirectory(tests)
add_subdirectory(neg-tests)
