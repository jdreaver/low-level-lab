# Adapts this tutorial, kinda
# (https://blog.podkalicki.com/how-to-compile-and-burn-the-code-to-stm32-chip-on-linux-ubuntu/)

CROSS_COMPILE = arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy

CFLAGS = -Wall -Wextra -pedantic -Wshadow -fanalyzer
# CFLAGS += -mlittle-endian -mthumb -mthumb-interwork # Not necessary?
CFLAGS += -mcpu=cortex-m4 # My board is NUCLEO-F401RE, which has a Cortex-M4 processor
LDFLAGS = -nostdlib

.PHONY: all
all: blink.bin

blink.bin: blink.elf
	$(OBJCOPY) -O binary $< $@

blink.elf: blink.o
	$(CC) $(LDFLAGS) -o $@ $<

blink.o: blink.c
	$(CC) $(CFLAGS) -c -o $@ $<

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
