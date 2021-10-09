project(inky-test)

add_executable(${PROJECT_NAME}  src/test.cpp
        src/eval_tests.cpp)

include_directories(${CMAKE_BINARY_DIR}/_deps/catch2-src/single_include)

target_include_directories(${PROJECT_NAME} PRIVATE "../core/src")
target_link_libraries(${PROJECT_NAME} inky-core fmt::fmt)