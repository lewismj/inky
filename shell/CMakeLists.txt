project(inky-repl)
find_package(fmt)

add_executable(${PROJECT_NAME} src/inky_sh.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE "../core/src")

target_link_libraries(${PROJECT_NAME} inky-core fmt::fmt)
