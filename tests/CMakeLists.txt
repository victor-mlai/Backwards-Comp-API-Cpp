set(TARGET_NAME bcAPI_tests)

add_executable(${TARGET_NAME}
        StructRenameTest.cpp
        NamespaceRenameTest.cpp
        )
target_include_directories(${TARGET_NAME} PRIVATE .)
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_20)
target_link_libraries(${TARGET_NAME} PRIVATE gtest_main)

if (MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE /WX /W4)
else ()
    target_compile_options(${TARGET_NAME} PRIVATE -Werror])
endif ()

if (BCAPI_BUILD_OLD_VERSION)
    target_compile_definitions(${TARGET_NAME} PRIVATE "OLD_CODE_ENABLED")
endif ()

include(GoogleTest)
gtest_discover_tests(${TARGET_NAME})
