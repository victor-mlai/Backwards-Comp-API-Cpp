include(compile_fail)
include(my_add_test)

file(GLOB test_files *.cpp)
foreach(test_file ${test_files})
    get_filename_component(target_name ${test_file} NAME_WE) # get file NAME W/o Ext
    string(APPEND target_name "_NEG")
    # negative tests should fail only after changing the code to the new version
    if(BC_API_CHANGED)
        compile_fail(${target_name} "${test_file}")
    else()
        # else, check that compilation succeeds
        my_add_test(${target_name} "${test_file}")
    endif()
    target_link_libraries(${target_name} PRIVATE gtest_main some_unstable_lib)
endforeach()
