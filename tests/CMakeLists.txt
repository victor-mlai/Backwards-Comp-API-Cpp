include(my_add_test)

file(GLOB test_files *Test.cpp)
foreach (test_file ${test_files})
    get_filename_component(target_name ${test_file} NAME_WE) # get file NAME W/o Ext
    my_add_test(${target_name} "${test_file}")
    target_link_libraries(${target_name} PRIVATE gtest_main some_unstable_lib)
    target_compile_features(${target_name} PRIVATE cxx_std_17)

    if (OLD_CODE_ENABLED)
        target_compile_definitions(${target_name} PRIVATE "OLD_CODE_ENABLED")
    endif ()
endforeach ()
