# Adapts these tutorials:
# - http://pandafruits.com/stm32_primer/stm32_primer_minimal.php
# - https://jeremyherbert.net/get/stm32f4_getting_started
# - https://freeelectron.ro/bare-metal-stm32-led-blink/
# - (for timer) https://vivonomicon.com/2018/05/20/bare-metal-stm32-programming-part-5-timer-peripherals-and-the-system-clock/

TARGETS = blink blink_timer
OBJS = $(patsubst %,%.o,$(TARGETS))
ELFS = $(patsubst %,%.elf,$(TARGETS))
BINS = $(patsubst %,%.bin,$(TARGETS))
FLASH_TARGET ?= blink

# My board is NUCLEO-F401RE, which has a Cortex-M4 processor
MCPU = cortex-m4

CROSS_COMPILE = arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)as
OBJCOPY = $(CROSS_COMPILE)objcopy
SIZE = $(CROSS_COMPILE)size

CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wshadow
CFLAGS += -fanalyzer
CFLAGS += -mthumb
CFLAGS += -mthumb-interwork
CFLAGS += -mcpu=$(MCPU)
CFLAGS += --specs=nosys.specs

# Run like "make DEBUG=0 ..." to disable debugging
DEBUG ?= 1
ifeq ($(DEBUG), 1)
  CFLAGS += -g3 -ggdb3 -O0
else
  CFLAGS += -Os
endif

# Linking with GCC requires more flags
LINK_WITH_GCC ?= 1
ifeq ($(LINK_WITH_GCC), 1)
  LD = $(CC)
  LDFLAGS += -mthumb
  LDFLAGS += -mthumb-interwork
  LDFLAGS += -mcpu=$(MCPU)
  LDFLAGS += --specs=nosys.specs
  LDFLAGS += -lgcc
  LDFLAGS += -lc
  # Linker map files are incredibly useful to debug the linker script and ensure
  # sections are getting loaded where you think they are.
  LDFLAGS += -Wl,-Map=$@.map
else
  LD = $(CROSS_COMPILE)ld
  LDFLAGS += -Map=$@.map
endif

.PHONY: all
all: $(BINS)

$(BINS): %.bin: %.elf
	$(OBJCOPY) -O binary $< $@

$(ELFS): %.elf: %.o vector_table.o linker.ld
	$(LD) $(LDFLAGS) -T $(filter %.ld,$^) -o $@ $(filter %.o,$^)
	$(SIZE) $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

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
	rm -f *.o *.bin *.elf *.map compile_commands.json

.PHONY: flash
flash: $(FLASH_TARGET).bin
	sudo st-flash write $? 0x8000000
	sudo st-flash reset

.PHONY: erase
erase:
	sudo st-flash erase
