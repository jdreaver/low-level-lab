add_library(common STATIC
  seven_segment_display.c
  tim2.c
  usart2.c
  user_button.c
  user_led.c
  vector_table.c
)

# TODO: I wish headers weren't separate INTERFACE library from above STATIC
# library
add_library(common_interface INTERFACE)
target_include_directories(common_interface INTERFACE .)
