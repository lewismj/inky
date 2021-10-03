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

FetchContent_Declare(
    utf8proc
    GIT_REPOSITORY https://github.com/JuliaStrings/utf8proc
    GIT_TAG v2.6.1
)

# Utf library from Julia project.
FetchContent_GetProperties(utf8proc)
if(NOT utf8proc_POPULATED)
    FetchContent_Populate(utf8proc)
    add_subdirectory(${utf8proc_SOURCE_DIR} ${utf8proc_BINARY_DIR})
endif()