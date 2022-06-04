CFLAGS=

# Run like "make DEBUG=1 ..." to turn on debugging symbols
DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS=-g3 -ggdb3 -DDEBUG -O0
endif

CC=gcc
CFLAGS+=-Wall
CFLAGS+=-Wextra
CFLAGS+=-pedantic

.PHONY: all
all: bin/assembler

assembler_sources := $(wildcard assembler/*.c)
assembler_headers := $(wildcard assembler/*.h)

bin/assembler: $(assembler_sources) $(assembler_headers)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(assembler_sources)

.PHONY: clean
clean:
	git clean -xfd
