add_library(some_unstable_lib INTERFACE)

target_include_directories(some_unstable_lib
        INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

if (BC_API_CHANGED)
    target_compile_definitions(some_unstable_lib INTERFACE "BC_API_CHANGED")
endif ()
