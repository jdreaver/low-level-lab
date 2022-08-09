function(stm32_executable TARGET)
  # Create executable
  add_executable(${TARGET} "${TARGET}.c")
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
