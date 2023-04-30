
set(target_name some_unstable_lib)

add_library(${target_name} INTERFACE ../neg-tests/ReturnTypeChangeTest.cpp)

target_include_directories(${target_name}
        INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

