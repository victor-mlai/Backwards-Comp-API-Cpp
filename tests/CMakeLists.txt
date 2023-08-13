include(my_add_test)

file(GLOB test_files *.cpp)
foreach (test_file ${test_files})
    get_filename_component(target_name ${test_file} NAME_WE) # get file NAME W/o Ext
    my_add_test(${target_name} "${test_file}")
    target_link_libraries(${target_name} PRIVATE gtest_main some_unstable_lib)
endforeach ()
