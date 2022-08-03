# Adapts this tutorial
# (https://create.arduino.cc/projecthub/milanistef/introduction-to-bare-metal-programming-in-arduino-uno-f3e2b4)
# to Arduino Mega (https://store.arduino.cc/products/arduino-mega-2560-rev3)

CROSS_COMPILE = avr-
CC = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy

MMCU = -mmcu=atmega2560
CFLAGS = -Wall -Wextra -pedantic -Wshadow -fanalyzer
CFLAGS += -Os $(MMCU)
LDFLAGS = $(MMCU)

.PHONY: all
all: blink.hex

# TODO: Do we need -R .eeprom? What about -j .text -j .data?
blink.hex: blink
	$(OBJCOPY) -O ihex -R .eeprom $< $@

blink: blink.o
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
	rm -f *.o *.hex blink

# N.B. avrdude man page and random internet posts suggest stk500v2 (which is
# also called wiring):
# - https://www.avrfreaks.net/forum/cannot-connect-avrdude-mega2560
# - https://kevintechnology.com/2012/06/15/programming-arduino-mega-using-avrdude.html
#
# I added the -D option (Disable auto erase for flash memory) after seeing an
# error without it and looking at the verbose Upload output from the Arduino IDE
# and noticing they use it.
.PHONY: flash
flash: blink.hex
	 avrdude -D -F -V -c wiring -p atmega2560 -P /dev/ttyACM0 -b 115200 -U flash:w:blink.hex
