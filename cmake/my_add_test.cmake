
function(my_add_test target_name test_file)
    add_executable(${target_name} ${test_file})

    if(MSVC)
        target_compile_options(${target_name} PRIVATE /WX /W4)
    else()
        target_compile_options(${target_name} PRIVATE -Wall)#-Werror)
    endif()

    add_test(NAME ${target_name}
            COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}" --target ${target_name} --config $<CONFIGURATION>)
endfunction()
