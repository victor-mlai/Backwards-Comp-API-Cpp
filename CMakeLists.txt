cmake_minimum_required(VERSION 3.14)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(BC_API_CHANGED "If checked, you will build & test with the API changed." ON)
message("INFO: BC_API_CHANGED " ${BC_API_CHANGED})

project(
	bcAPI
	HOMEPAGE_URL "https://github.com/victorlaurentiu/Backwards-Comp-API-Cpp"
	DESCRIPTION "Tricks in C++ for making API changes as non-breaking and boilerplate free as possible"
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(include)


enable_testing()

include(FetchContent)
FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_subdirectory(tests)
add_subdirectory(neg-tests)
