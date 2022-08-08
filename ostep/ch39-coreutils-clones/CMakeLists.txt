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

add_compile_options(-Wall -Wextra -pedantic -Wshadow -fanalyzer -flto)

add_executable(
  mystat
  mystat.c
  file_utils.c
)

add_executable(
  myls
  myls.c
  file_utils.c
)
