# STM32

Bits of code for STM32 microcontrollers. I bought an [STMF401RE Nucleo
board](https://www.st.com/en/evaluation-tools/nucleo-f401re.html).

## References

- https://github.com/STMicroelectronics/STM32CubeF4 Official STM32 headers (ARM
  CORE, CMSIS, HAL, etc). Note that the ARM CORE here is a bit out of date
  (intentional maybe?)
- https://github.com/ARM-software/CMSIS_5 Official CMSIS 5 headers from ARM
- https://www.st.com/en/microcontrollers-microprocessors/stm32f401re.html#documentation
  STMF401RE docs (datasheet, reference manual, programming manual, etc)

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
