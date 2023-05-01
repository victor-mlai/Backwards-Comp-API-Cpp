add_library(some_unstable_lib INTERFACE)

target_include_directories(some_unstable_lib
        INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

target_compile_features(some_unstable_lib INTERFACE cxx_std_17)
