include(compile_fail)
include(my_add_test)

file(GLOB test_files *Test.cpp)
foreach(test_file ${test_files})
    get_filename_component(target_name ${test_file} NAME_WE) # get file NAME W/o Ext
    string(APPEND target_name "_NEG")
    # negative tests should fail only after changing the code to the new version
    if(NOT OLD_CODE_ENABLED)
        compile_fail(${target_name} "${test_file}")
    else()
        #compile_succeeds
        my_add_test(${target_name} "${test_file}")
        target_compile_definitions(${target_name} PRIVATE "OLD_CODE_ENABLED")
    endif()
    target_link_libraries(${target_name} PRIVATE some_unstable_lib)
endforeach()
