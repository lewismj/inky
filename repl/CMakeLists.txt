project(inky-repl)

include_directories(${CMAKE_BINARY_DIR}/_deps/fmt-src/include) # fmt library

add_executable(${PROJECT_NAME} src/repl.cpp src/shell.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE "../core/src")

target_link_libraries(${PROJECT_NAME} inky-core fmt::fmt)

