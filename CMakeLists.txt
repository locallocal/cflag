cmake_minimum_required(VERSION 3.10)
project(cflag LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

option(CFLAG_BUILD_TESTS "Build the cflag test suite" OFF)
option(CFLAG_ENABLE_COVERAGE "Enable GCC/Clang coverage instrumentation" OFF)

add_library(cflag STATIC
    src/bool.cc
    src/cflag.cc
    src/float.cc
    src/int.cc
    src/string.cc
)
target_include_directories(cflag PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
)

if (CFLAG_ENABLE_COVERAGE)
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(cflag PRIVATE --coverage -fno-omit-frame-pointer)
        target_link_libraries(cflag PUBLIC --coverage)
    else()
        message(FATAL_ERROR "Coverage is only supported with GCC or Clang")
    endif()
endif()

add_executable(example example/example.cc)
target_link_libraries(example PRIVATE cflag)

if (CFLAG_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

