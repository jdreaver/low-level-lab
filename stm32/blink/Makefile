# Adapts these tutorials:
# - http://pandafruits.com/stm32_primer/stm32_primer_minimal.php
# - https://jeremyherbert.net/get/stm32f4_getting_started
# - https://freeelectron.ro/bare-metal-stm32-led-blink/
# - https://blog.podkalicki.com/how-to-compile-and-burn-the-code-to-stm32-chip-on-linux-ubuntu/

CROSS_COMPILE = arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
AS = $(CROSS_COMPILE)as
OBJCOPY = $(CROSS_COMPILE)objcopy

CFLAGS = -Wall -Wextra -pedantic -Wshadow -fanalyzer -g3 -ggdb3 -O0
CFLAGS += -mthumb -mcpu=cortex-m4 # My board is NUCLEO-F401RE, which has a Cortex-M4 processor

.PHONY: all
all: blink.bin

blink.bin: blink.elf
	$(OBJCOPY) -O binary $< $@

blink.elf: blink.o linker.ld
	$(LD) -T linker.ld -o $@ blink.o

blink.o: blink.c
	$(CC) $(CFLAGS) -c -o $@ $<

# crt.o: crt.S
# 	$(AS) -o $@ $<

.PHONY: compile_commands
compile_commands:
	$(MAKE) clean
	bear -- $(MAKE)
  # We need to tell bear to exclude compiler wrappers, or we'll have duplicate,
  # incorrect entries in compile_commands.json. See:
  # - https://github.com/rizsotto/Bear/wiki/Troubleshooting#the-output-has-duplicate-entries
  # - https://github.com/NixOS/nixpkgs/issues/20056
  # - https://github.com/rizsotto/Bear/issues/321
  #
  # An alternative solution is to use the config file to pass in the compiler
  # wrapper under compilation.compilers_to_exclude.
	jq 'map(select(.arguments[0] | contains("wrapper") | not))' compile_commands.json > tmp_compile_commands.json
	mv tmp_compile_commands.json compile_commands.json

.PHONY: clean
clean:
	rm -f *.o *.bin *.elf

.PHONY: flash
flash: blink.bin
	sudo st-flash write blink.bin 0x8000000

.PHONY: erase
erase:
	sudo st-flash erase
