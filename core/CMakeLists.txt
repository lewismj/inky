project(inky-core)

# define sources/headers:
set (SOURCES    src/parser.cpp
                src/value.cpp
                src/environment.cpp
                src/eval.cpp
                src/builtin.cpp
                src/core.h)

set (HEADERS src/either.h
             src/core.h
             src/parer.h
             src/values.h
             src/environment.h
             src/eval.h
             src/builtin.h
        )

include_directories(${CMAKE_BINARY_DIR}/_deps/fmt-src/include) # fmt library

# library definition:
add_library(${PROJECT_NAME} ${SOURCES})

# only need to make repl.h available in the headers folder in distribution.
set_target_properties(inky-core PROPERTIES PUBLIC_HEADER "src/core.h")

install (   TARGETS ${PROJECT_NAME}
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
            PUBLIC_HEADER DESTINATION include
            RUNTIME DESTINATION bin)
