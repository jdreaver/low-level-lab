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

# My board is NUCLEO-F401RE, which has a Cortex-M4 processor
#
# TODO: This should probably be in the toolchain config somehow, including
# setting -D${BOARD_NAME}
set(BOARD_NAME "STM32F401xE" CACHE STRING "Name of target board")
set(MCPU "cortex-m4" CACHE STRING "Name of target CPU")
add_compile_options("-D${BOARD_NAME}")

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
add_compile_options(-mthumb)
add_compile_options(-mthumb-interwork)
add_compile_options(-mcpu=${MCPU})
add_compile_options(--specs=nosys.specs)
# Generate sections so we can -gc-sections them in linker
add_compile_options(-ffunction-sections -fdata-sections)
# TODO: Set optimize level based on build type (Debug/Release)
# add_compile_options(-g3 -ggdb3 -O0)
add_compile_options(-Os)

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

# Build libraries
add_subdirectory(common)
add_subdirectory(vendor)
target_link_libraries(common PRIVATE vendor)

cmake_path(SET LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/common/linker.ld")

function(stm32_executable TARGET)
  # Set executable as main.c file for target name
  cmake_path(SET main_path "${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/main.c")
  add_executable(${TARGET} "${main_path}")
  target_link_libraries(${TARGET} PRIVATE common common_interface vendor)

  # Add linker script
  target_link_options(${TARGET} PRIVATE -T "${LINKER_SCRIPT}")
  set_target_properties(${TARGET} PROPERTIES LINK_DEPENDS "${LINKER_SCRIPT}")

  stm32_generate_binary_file(${TARGET})
  stm32_print_size_of_target(${TARGET})
endfunction()

stm32_executable(blink)
stm32_executable(blink_timer)
stm32_executable(one_digit_7_segment_counter)
