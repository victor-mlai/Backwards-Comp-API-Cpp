cmake_minimum_required(VERSION 3.14)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(BC_API_CHANGED "If checked, you will build & test with the API changed." ON)
message("INFO: BC_API_CHANGED " ${BC_API_CHANGED})

project(
	bcAPI
	HOMEPAGE_URL "https://github.com/victor-mlai/Backwards-Comp-API-Cpp"
	DESCRIPTION "Tricks in C++ for making API changes as non-breaking and boilerplate free as possible"
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	# fix for: undefined function '_S_fractional_width' cannot be used in a constant expression
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -std=c++20")
endif()

add_subdirectory(some_unstable_lib)


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
