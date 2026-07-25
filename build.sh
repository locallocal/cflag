#!/usr/bin/env bash

set -euo pipefail

readonly BUILD_DIR="build"
readonly COVERAGE_DIR="cov"

function configure_and_build {
    cmake -S . -B "${BUILD_DIR}" "$@"
    cmake --build "${BUILD_DIR}" --parallel
}

function build_release {
    configure_and_build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCFLAG_BUILD_TESTS=OFF \
        -DCFLAG_ENABLE_COVERAGE=OFF
}

function build_debug {
    configure_and_build \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCFLAG_BUILD_TESTS=OFF \
        -DCFLAG_ENABLE_COVERAGE=OFF
}

function run_tests {
    configure_and_build \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCFLAG_BUILD_TESTS=ON \
        -DCFLAG_ENABLE_COVERAGE=OFF
    ctest --test-dir "${BUILD_DIR}" --output-on-failure
}

function run_coverage {
    if ! command -v lcov >/dev/null 2>&1 || ! command -v genhtml >/dev/null 2>&1; then
        echo "lcov and genhtml are required for coverage." >&2
        exit 1
    fi

    configure_and_build \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCFLAG_BUILD_TESTS=ON \
        -DCFLAG_ENABLE_COVERAGE=ON
    ctest --test-dir "${BUILD_DIR}" --output-on-failure
    lcov --capture --directory "${BUILD_DIR}" --output-file "${BUILD_DIR}/cflag.info"
    genhtml "${BUILD_DIR}/cflag.info" --output-directory "${COVERAGE_DIR}"
}

function clean {
    rm -rf "${BUILD_DIR}" "${COVERAGE_DIR}"
}

function print_usage {
    echo "Usage: ./build.sh [--release|--debug|--test|--cov|--clean|--help]"
}

case "${1:---release}" in
    --release)
        build_release
        ;;
    --debug)
        build_debug
        ;;
    --test)
        run_tests
        ;;
    --cov)
        run_coverage
        ;;
    --clean)
        clean
        ;;
    --help)
        print_usage
        ;;
    *)
        print_usage >&2
        exit 1
        ;;
esac
