cmake_minimum_required(VERSION 3.19)

# Dependencies

include(FetchContent)

# Format {fmt} library, type-safe printf.
FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 8.0.1
)

FetchContent_GetProperties(fmt)
if(NOT fmt_POPULATED)
    FetchContent_Populate(fmt)
    add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR})
endif()


# Testing framework, Catch2
FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v2.13.7
)

if (NOT catch2_POPULATED)
    FetchContent_Populate(catch2)
    add_subdirectory (${catch2_SOURCE_DIR} ${catch2_BINARY_DIR})
endif()