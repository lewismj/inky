project(inky-core)

# add external include paths:
include_directories(${CMAKE_BINARY_DIR}/_deps/fmt-src/include)

set (SOURCES src/repl.cpp src/environment.cpp src/parser.cpp src/value.cpp)
set (HEADERS src/repl.h src/environment.h src/parser.h src/value.h src/parser_t.h)

add_library(${PROJECT_NAME} ${SOURCES})

set_target_properties(inky-core PROPERTIES PUBLIC_HEADER "src/repl.h")

install (   TARGETS ${PROJECT_NAME}
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
            PUBLIC_HEADER DESTINATION include
            RUNTIME DESTINATION bin)