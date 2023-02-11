set(TARGET_NAME bcAPI_tests)

add_executable(${TARGET_NAME}
  first_test.cpp
)
target_include_directories(${TARGET_NAME} PRIVATE .)
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_20)
target_link_libraries(${TARGET_NAME} PRIVATE gtest_main)

if (MSVC)
  target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()


include(GoogleTest)
gtest_discover_tests(${TARGET_NAME})
