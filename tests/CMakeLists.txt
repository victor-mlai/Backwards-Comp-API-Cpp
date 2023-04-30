include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG b796f7d44681514f58a683a3a71ff17c94edb0c1 # release-1.13.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
include(googletest)

include(my_add_test)

file(GLOB test_files *Test.cpp)
foreach (test_file ${test_files})
    get_filename_component(target_name ${test_file} NAME_WE) # get file NAME W/o Ext
    my_add_test(${target_name} "${test_file}")
    target_link_libraries(${target_name} PRIVATE gtest_main some_unstable_lib)
    target_compile_features(${target_name} PRIVATE cxx_std_17)

    if (BCAPI_BUILD_OLD_VERSION)
        target_compile_definitions(${target_name} PRIVATE "OLD_CODE_ENABLED")
    endif ()
endforeach ()
