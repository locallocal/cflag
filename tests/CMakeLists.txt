find_package(GTest REQUIRED)

add_executable(cflag-test
    test_cflag_bool.cc
    test_cflag_builtin_types.cc
    test_cflag_common.cc
    test_cflag_fixed_width.cc
    test_cflag_float.cc
    test_cflag_int.cc
    test_cflag_string.cc
    test_cflag_template.cc
)
target_link_libraries(cflag-test PRIVATE GTest::Main cflag::cflag)

include(GoogleTest)
gtest_discover_tests(cflag-test)
