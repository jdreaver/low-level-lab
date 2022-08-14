add_library(common STATIC
  hd44780u_lcd.c
  seven_segment_display.c
  system_clock.c
  tim2.c
  usart2.c
  user_button.c
  user_led.c
  vector_table.c
)

# This tells any target that depends on this library to include this directory
# to search for headers.
target_include_directories(common INTERFACE .)
