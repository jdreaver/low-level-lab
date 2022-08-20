# Testing out CMake against this module

project(ch39-coreutils-clones LANGUAGES C)
cmake_minimum_required(VERSION 3.20)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_custom_target(
    copy-compile-commands ALL
    ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_CURRENT_LIST_DIR}
    )

# This is how you set -flto in CMake without pain
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

add_compile_options(-Wall -Wextra -pedantic -Wshadow -fanalyzer)
add_compile_options("$<$<CONFIG:Release>:-O2>")
add_compile_options("$<$<CONFIG:Debug>:-g3;-ggdb3;-O0;-fsanitize=address,leak,undefined;-fsanitize-undefined-trap-on-error>")
add_link_options("$<$<CONFIG:Debug>:-fsanitize=address,leak,undefined;-fsanitize-undefined-trap-on-error>")

add_library(file_utils STATIC file_utils.c)

add_executable(mystat mystat.c)
target_link_libraries(mystat PRIVATE file_utils)

add_executable(myls myls.c)
target_link_libraries(myls PRIVATE file_utils)
