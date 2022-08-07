# Adapts these tutorials:
# - http://pandafruits.com/stm32_primer/stm32_primer_minimal.php
# - https://jeremyherbert.net/get/stm32f4_getting_started
# - https://freeelectron.ro/bare-metal-stm32-led-blink/
# - (for timer) https://vivonomicon.com/2018/05/20/bare-metal-stm32-programming-part-5-timer-peripherals-and-the-system-clock/

TARGETS += blink
TARGETS += blink_timer
TARGETS += one_digit_7_segment_counter

BINS = $(addsuffix /main.bin,$(TARGETS))
ELFS = $(patsubst %.bin,%.elf,$(BINS))

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
CFLAGS += -I./vendor
CFLAGS += -I./common
# This tells the vendored header file stm32f4xx.h which board we are using.
CFLAGS += -DSTM32F401xE

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
  LDFLAGS += -nostartfiles
  # Linker map files are incredibly useful to debug the linker script and ensure
  # sections are getting loaded where you think they are.
  LDFLAGS += -Wl,-Map=$@.map
else
  LD = $(CROSS_COMPILE)ld
  LDFLAGS += -nostartfiles
  LDFLAGS += -Map=$@.map
endif

# TODO: Not all programs need all of these object files. Figure out per-target
# object dependencies or some kind of link-time optimization to remove unused
# symbols.
LIB_OBJS += common/vector_table.o
LIB_OBJS += common/user_button.o

# Automatically generate dependencies. -MMD generates non-system header
# dependencies. -MP creates an empty rule for generating each dependency. See
# http://www.microhowto.info/howto/automatically_generate_makefile_dependencies.html
CPPFLAGS+=-MMD -MP

.PHONY: all
all: $(BINS)

$(BINS): %.bin: %.elf
	$(OBJCOPY) -O binary $< $@

$(ELFS): %.elf: %.o common/linker.ld $(LIB_OBJS)
	$(LD) $(LDFLAGS) -T $(filter %.ld,$^) -o $@ $(filter %.o,$^)
	$(SIZE) $@

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $^

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
	find . -type f \( -name '*.o' -o -name '*.d' -o -name '*.elf' -o -name '*.map' -o -name '*.bin' \) -delete
	rm -f compile_commands.json

ifeq ($(MAKECMDGOALS),flash)
ifeq ($(FLASH_TARGET),)
  $(error "FLASH_TARGET is empty!")
endif
endif

.PHONY: flash
flash: $(FLASH_TARGET)/main.bin
	sudo st-flash write $? 0x8000000
	sudo st-flash reset

.PHONY: erase
erase:
	sudo st-flash erase

# Include all of the dependency files generated from -MD and -MP. Purposely at
# the bottom of this Makefile so these don't supersede the default target on
# accident.
-include $(DEPS)
