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

# TODO: This should probably be in the toolchain config somehow, including
# setting -D${BOARD_NAME}
set(BOARD_NAME "STM32F401xE" CACHE STRING "Name of target board")
set(MCPU "cortex-m4" CACHE STRING "Name of target CPU")

# TODO: Is -D${BOARD_NAME supposed to be per-target?
add_compile_options("-D${BOARD_NAME}")

add_compile_options(-Wall)
add_compile_options(-Werror)
add_compile_options(-Wextra)
add_compile_options(-Wshadow)
add_compile_options(-Wstack-usage=255) # Warn if stack is too large (in bytes)
add_compile_options(-fanalyzer)
# Leave behind intermediate artifacts. Super useful for debugging.
# add_compile_options(-save-temps=obj)
add_compile_options(-mthumb)
add_compile_options(-mthumb-interwork)
add_compile_options(-mcpu=${MCPU})
add_compile_options(--specs=nosys.specs)
# Generate sections so we can -gc-sections them in linker
add_compile_options(-ffunction-sections -fdata-sections)

# TODO: Add linker script, and make sure changing linker script causes
# recompilation.
add_link_options(-mthumb)
add_link_options(-mthumb-interwork)
add_link_options(-mcpu=${MCPU})
add_link_options(--specs=nosys.specs)
add_link_options(-Wl,--gc-sections) # Clean up unused sections, particularly from -ffunction-sections -fdata-sections
add_link_options(-lgcc)
add_link_options(-lc)
add_link_options(-nostartfiles)
# TODO: Linker map files are incredibly useful to debug the linker script and ensure
# sections are getting loaded where you think they are.
# add_link_options(-Wl,-Map=$@.map)

add_subdirectory(common)

add_subdirectory(vendor)
target_link_libraries(common PRIVATE vendor)

add_executable(blink blink/main.c)
target_link_libraries(blink PRIVATE common common_interface vendor)
