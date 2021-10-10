project(inky-core)

# add external include paths:
include_directories(${CMAKE_BINARY_DIR}/_deps/fmt-src/include) # fmt library

# define sources/headers:
set (SOURCES src/repl.cpp src/environment.cpp src/parser.cpp src/value.cpp src/eval.cpp src/builtin.cpp)
set (HEADERS    src/repl.h
                src/environment.h
                src/parser.h
                src/value.h
                src/types.h
                src/eval.h,
                src/builtin.h )

# library definition:
add_library(${PROJECT_NAME} ${SOURCES})

# only need to make repl.h available in the headers folder in distribution.
set_target_properties(inky-core PROPERTIES PUBLIC_HEADER "src/repl.h")

install (   TARGETS ${PROJECT_NAME}
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
            PUBLIC_HEADER DESTINATION include
            RUNTIME DESTINATION bin)