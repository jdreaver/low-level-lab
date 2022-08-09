cmake_minimum_required(VERSION 3.23)

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

# TODO: Figure out how to propertly handle all of these flags. Some of them are
# based on target (like -mcpu, -mthumb, -D${BOARD_NAME}, etc). Some are really
# just for development and shouldn't be mandatory, optimization level should
# vary based on release type (Debug/Release), etc.
add_compile_options(-Wall)
add_compile_options(-Werror)
add_compile_options(-Wextra)
add_compile_options(-Wshadow)
add_compile_options(-Wstack-usage=255) # Warn if stack is too large (in bytes)
add_compile_options(-fanalyzer)
# Leave behind intermediate artifacts. Super useful for debugging.
# add_compile_options(-save-temps=obj)
# Generate sections so we can -gc-sections them in linker
add_compile_options(-ffunction-sections -fdata-sections)
add_compile_options("$<$<CONFIG:Release>:-Os>")
add_compile_options("$<$<CONFIG:Debug>:-g3;-ggdb3;-O0>")

add_link_options(--specs=nosys.specs)
add_link_options(-Wl,--gc-sections) # Clean up unused sections, particularly from -ffunction-sections -fdata-sections
add_link_options(-lgcc)
add_link_options(-lc)
add_link_options(-nostartfiles)
# TODO: Linker map files are incredibly useful to debug the linker script and ensure
# sections are getting loaded where you think they are.
# add_link_options(-Wl,-Map=$@.map)

# Build libraries
add_subdirectory(common)
add_subdirectory(vendor)
target_link_libraries(common PRIVATE vendor)

cmake_path(SET LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/common/linker.ld")

add_subdirectory(bin)
