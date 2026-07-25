cmake_minimum_required(VERSION 3.10)
project(cflag LANGUAGES CXX)

set(CFLAG_IS_TOP_LEVEL OFF)
if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(CFLAG_IS_TOP_LEVEL ON)
endif()

if (CFLAG_IS_TOP_LEVEL AND NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

option(CFLAG_BUILD_EXAMPLE "Build the cflag example" ${CFLAG_IS_TOP_LEVEL})
option(CFLAG_BUILD_TESTS "Build the cflag test suite" OFF)
option(CFLAG_ENABLE_COVERAGE "Enable GCC/Clang coverage instrumentation" OFF)

add_library(cflag INTERFACE)
add_library(cflag::cflag ALIAS cflag)
target_include_directories(cflag INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
)
target_compile_features(cflag INTERFACE cxx_std_11)

if (CFLAG_ENABLE_COVERAGE)
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(cflag INTERFACE --coverage -fno-omit-frame-pointer)
        target_link_libraries(cflag INTERFACE --coverage)
    else()
        message(FATAL_ERROR "Coverage is only supported with GCC or Clang")
    endif()
endif()

if (CFLAG_BUILD_EXAMPLE)
    add_executable(example example/example.cc)
    target_link_libraries(example PRIVATE cflag::cflag)
    set_target_properties(example PROPERTIES
        CXX_EXTENSIONS OFF
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
    )
endif()

if (CFLAG_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
