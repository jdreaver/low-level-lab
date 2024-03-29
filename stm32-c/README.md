# STM32 C Code

Bits of code for STM32 microcontrollers. I bought an [STMF401RE Nucleo
board](https://www.st.com/en/evaluation-tools/nucleo-f401re.html).

## Usage

1. Configure/build via CMake using `make configure` and `make build`
2. Flash a target to a board plugged in via USB with `make FLASH_TARGET=build/path/to.bin flash`

## References

- https://github.com/STMicroelectronics/STM32CubeF4 Official STM32 headers (ARM
  CORE, CMSIS, HAL, etc). Note that the ARM CORE here is a bit out of date
  (intentional maybe?)
- https://github.com/ARM-software/CMSIS_5 Official CMSIS 5 headers from ARM
- https://www.st.com/en/microcontrollers-microprocessors/stm32f401re.html#documentation
  STMF401RE docs (datasheet, reference manual, programming manual, etc)

## STM32F401RE Special Pins

I generally avoid these pins because they are special:

- PA5: User LED
- PA2/PA3: Used for UART and ST-Link (PA2 is `USART2_RX`, PA3 is `USART2_TX`)
- PB3: JTDO in floating state
- PB4: NJTRST in pull-up
- PA0/PA1/PA4/PB0/PC1/PB9/PC0/PB8: These are on the CN8 analog connector.
  - PB8/PB9 are for I2C1. PB9 is `I2C1_SDA`, PB8 is `I2C1_SCL`
- PC14/PC15: These can be GPIO or LSE pins, but I noticed when I used them as
  GPIO pins I saw a faint voltage even when LSEON was unset.
  - PC13 also seems fishy
- PA13/PA14: PA13 and PA14 share with SWD signals connected to ST-LINK/V2-1, it
  is not recommended to use them as IO pins if the ST-LINK part is not cut.
  - PA15: JTDI in pull-up
  - PA14: JTCK/SWCLK in pull-down
  - PA13: JTMS/SWDAT in pull-up

## Debugging

1. Make sure to compile with `-g3 -ggdb3 -O0` and all that jazz
2. Flash program to board
3. In a terminal, run `sudo st-util`. This sets up a gdb server on port 4242
4. Run `gdb <target>.elf` (make sure you use the `.elf` file so we have debug
   info!)
5. Run `target extended-remote :4242`. You should connect and either pop into
   some infinite loop, or perhaps some handler.
6. Add a breakpoint somewhere, and run `continue` to get to it. Alternatively,
   you can add a breakpoint to the reset handler, run `continue`, and then press
   the reset button on the board to get to the reset handler.

## Vendored headers

Both ARM and ST Micro provide header files. ARM provides them for Cortex, and ST
provides them for specific boards. These headers are very useful so I don't have
to look up register locations every time I want to use a register, and that's
about all I use them for. They are located at:

- https://github.com/ARM-software/CMSIS_5
- https://github.com/STMicroelectronics/STM32CubeF4 (for STM32F4xxx boards)

I have a script to download these at `vendor/update-vendor-headers.sh`. I avoid
the HAL like the plague because it looks stupid complex. I also provide my own
boot files and linker scripts for more control.

## CMake

I used to have a _fairly_ simple Makefile, but I hit a few snags just using
`make` that were cleanly solved by CMake:
- I wanted to segregate release/debug builds without clobbering build files, but
  didn't want to deal with refactoring my Makefile to support separate build
  artifacts from source.
- My `make clean` was error-prone because, again, I had build artifacts
  alongside my source.
- I clumped all my `CFLAGS` and `LDFLAGS` together for all targets by trial an
  error. This wouldn't scale to multiple board.
- I wanted to refactor how I did per-board header and source files, but didn't
  want to deal with refactoring my Makefile to accommodate it.
- I wanted to add unit tests somehow, but not deal with, again, segregating
  builds and dealing with a more complex linking setup.

Resources:
- I read [Professional CMake](https://crascit.com/professional-cmake/), which
  helped a ton.
- Good CMake example for stm32: <https://github.com/ObKo/stm32-cmake>. That repo
  support tons of different boards, HAL, etc and is more complex than I need,
  but it is good inspiration.

## Tutorials, Getting Started

All of the official STM32 guides encourage you to use a super bloated IDE called
[STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html). It is
based on Eclipse, very wizard-driven, and produces awful code based on their HAL
library. I looked far and wide for a simple explanation of how to run a very
simple program on my STM32 board, and after reading probably 30 tutorials, I
finally found some that work:

- https://vivonomicon.com/category/stm32_baremetal_examples/
  - Best explanations by far of bare metal STM32 programming. (Make sure to
    click Older Posts button at the bottom)
- https://www.st.com/en/evaluation-tools/nucleo-f401re.html
  - Great overall explanation of the basics. Split over 3 parts.
- http://pandafruits.com/stm32_primer/stm32_primer_minimal.php
  - This is the main tutorial that actually "worked" for me. The only difference
    is I am using a different board and I used `st-flash` to flash the board.
- https://freeelectron.ro/bare-metal-stm32-led-blink/
  - This tutorial is nice and simple, and explained how to find the GPIO/RCC
    values I needed. However, it uses a much older board, has a poor explanation
    of the vector table (I thought it was magic nonsense), and I couldn't
    actually get it to work.
- https://mcla.ug/blog/how-to-flash-an-led.html
  - Creates a blink program in ARM assembly! Also a great explanation of the
    vector table and linker script.
- https://jeremyherbert.net/get/stm32f4_getting_started
  - Quite old, and also a good GPIO explanation, but uses the ST-provided header
    files and library, so I didn't like it.
- https://www.youtube.com/c/FastbitEmbeddedBrainAcademy/playlists
  - Superb Youtube channel explaining low level bits of STM32 in detail. I
    actually found this after I got my `blink` working, but it would have
    explained everything if I saw it first. (This person also has some great
    [Udemy courses](https://www.udemy.com/user/kiran-nayak-2/) too.)
- [The most thoroughly commented linker script
  (probably)](https://blog.thea.codes/the-most-thoroughly-commented-linker-script/)

The key bit I was missing was the vector table. The vector table must start at
0x08000000, the first 4 byte entry is the location of the initial stack pointer,
and the next 4 byte entry (at 0x08000004) is the location of the reset interrupt
handler, which is what should basically point at my `main`. This key bit is what
was lost on me, and what a lot of the tutorials that had simple ARM assembly
files didn't explain at all. I had functions like `_start`, `main`, `_reset`,
etc thinking the names were special, but in reality the location of the pointer
to those functions is special, and it is why the assembly solutions had `.word
0x20001000`, etc or had linker scripts that specifically set up the vector
table.

## TODO

- Make an I2C scanner
  - https://learn.adafruit.com/scanning-i2c-addresses/arduino
  - Good article on STM32 I2C without HAL https://controllerstech.com/stm32-i2c-configuration-using-registers/
    - Video for article: https://www.youtube.com/watch?v=usvAIEdp_I8&list=PLfIJKC1ud8gi_y7yQivRMi9RARPRE8fmh&index=4
  - https://stackoverflow.com/questions/24078696/configuring-stm32-output-ports-for-i2c
- Make DS1307 module to practice I2C
  - https://datasheets.maximintegrated.com/en/ds/DS1307.pdf
- Create snake game w/ OLED screen and joystick. Could put score/status on LCD
  screen so we have more space for actual game (or put score/status on yellow
  part, game on blue part)
  - https://www.uctronics.com/tools-and-accessories/electrical-testing-tools/uctronics-0-96-inch-oled-module-12864-128x64-yellow-blue-ssd1306-driver-i2c-serial-self-luminous-display-board-for-arduino-raspberry-pi.html
    - https://www.uctronics.com/download/Amazon/U602602.pdf
  - https://github.com/UCTRONICS/SSD1306
  - Might be actual datasheet?
    https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
  - Good example library (for Arduino) https://github.com/greiman/SSD1306Ascii
  - Another great library from Adafruit (again, Arduino)
    https://github.com/adafruit/Adafruit_SSD1306
- Get bigger display, like https://www.amazon.com/dp/B073R7Q8FF
- Build system
  - (See TODOs in CMakeLists.txt files)
  - Read about CMake modules and consider putting more function definitions in
    `cmake/`
  - Consider having platform/board-dependent code in separate subdirectories
    instead of wrapping in C preprocessor #ifdef
    - Instead of using `-D${BOARD_NAME}`, consider using flag
      `-DBOARD_NAME=${BOARD_NAME}`
- Try out Rust for STM32 (this blog post might be old but it is good inspiration
  <https://vivonomicon.com/2019/05/23/hello-rust-blinking-leds-in-a-new-language/>)
  - Also https://docs.rust-embedded.org/book/intro/index.html
