cmake_minimum_required (VERSION 3.14)


project(bcAPI
	HOMEPAGE_URL "https://github.com/victorlaurentiu/Backwards-Comp-API-Cpp"
    DESCRIPTION "Tricks in C++ for making API changes as non-breaking, boilerplate free and backwards compatible as possible"
    LANGUAGES CXX
)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
		googletest
		GIT_REPOSITORY https://github.com/google/googletest.git
		GIT_TAG        703bd9caab50b139428cea1aaff9974ebee5742e # release-1.10.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_subdirectory(tests)
