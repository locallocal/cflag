# include(FetchContent)
# FetchContent_Declare(
#     googletest
#     URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
#)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(googletest)

aux_source_directory(. tests_srcs)

enable_testing()

add_executable(cflag-test "${tests_srcs}")

target_link_libraries(cflag-test gtest_main gtest cflag pthread)

include(GoogleTest)
gtest_discover_tests(cflag-test)
