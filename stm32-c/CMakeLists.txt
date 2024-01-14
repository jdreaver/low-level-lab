cmake_minimum_required(VERSION 3.24)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/stm32.cmake)

project(stm32 LANGUAGES C)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_custom_target(
    copy-compile-commands ALL
    ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_CURRENT_LIST_DIR}
)

# Set global compile options. (It could be argued that some of these should be
# set in preset configs, but whatever, this is a private project)
add_compile_options(-Wall)
add_compile_options(-Werror)
add_compile_options(-Wextra)
add_compile_options(-Wshadow)
add_compile_options(-Wextra)
add_compile_options(-Wstack-usage=255) # Warn if stack is too large (in bytes)
add_compile_options(-fanalyzer)
# Leave behind intermediate artifacts. Super useful for debugging.
# add_compile_options(-save-temps=obj)
# Generate sections so we can -gc-sections them in linker
add_compile_options(-ffunction-sections -fdata-sections)
add_compile_options("$<$<CONFIG:Release>:-Os>")
add_compile_options("$<$<CONFIG:Debug>:-g3;-ggdb3;-O0>")

# Build libraries
add_subdirectory(common)
add_subdirectory(vendor)
target_link_libraries(common PRIVATE vendor)

add_subdirectory(bin)
