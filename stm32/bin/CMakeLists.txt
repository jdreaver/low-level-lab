cmake_path(SET linker_script "${CMAKE_SOURCE_DIR}/common/linker.ld")

function(stm32_executable TARGET)
  # Create executable
  add_executable(${TARGET} "${TARGET}.c")
  target_link_libraries(${TARGET} PRIVATE common vendor)

  # Clean up unused sections, particularly from -ffunction-sections -fdata-sections
  target_link_options(${TARGET} PRIVATE -Wl,--gc-sections)
  target_link_options(${TARGET} PRIVATE -lgcc)
  target_link_options(${TARGET} PRIVATE -lc)
  target_link_options(${TARGET} PRIVATE -nostartfiles)
  # Linker map files are incredibly useful to debug the linker script and ensure
  # sections are getting loaded where you think they are.
  target_link_options(${TARGET} PRIVATE -Wl,-Map=${TARGET}.map)

  # Add linker script
  target_link_options(${TARGET} PRIVATE -T "${linker_script}")
  set_target_properties(${TARGET} PROPERTIES LINK_DEPENDS "${linker_script}")

  stm32_generate_binary_file(${TARGET})
  stm32_print_size_of_target(${TARGET})
  stm32_flash_target(${TARGET})
endfunction()

stm32_executable(blink)
stm32_executable(blink_timer)
stm32_executable(lcd_hello)
stm32_executable(one_digit_7_segment_counter)
