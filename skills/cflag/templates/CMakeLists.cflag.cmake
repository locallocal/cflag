# Add cflag to a CMake project with FetchContent. Pin GIT_TAG to a release tag
# or commit hash before committing this.
include(FetchContent)

FetchContent_Declare(
    cflag
    GIT_REPOSITORY https://github.com/locallocal/cflag.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(cflag)

# Then, for each target that includes cflag.h:
# target_link_libraries(<target> PRIVATE cflag::cflag)
