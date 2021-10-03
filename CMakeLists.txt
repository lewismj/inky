cmake_minimum_required(VERSION 3.19)
project(inky)

set(CMAKE_CXX_STANDARD 20)

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif(UNIX)


# inky
add_subdirectory(extern) # get external dependencies.
add_subdirectory(core)   # core system
add_subdirectory(shell)   # executable.