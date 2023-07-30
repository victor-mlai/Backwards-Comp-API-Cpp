add_library(some_unstable_lib INTERFACE)

target_include_directories(some_unstable_lib
        INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

target_compile_features(some_unstable_lib INTERFACE cxx_std_20)

if (OLD_CODE_ENABLED)
    target_compile_definitions(some_unstable_lib INTERFACE "OLD_CODE_ENABLED")
endif ()
