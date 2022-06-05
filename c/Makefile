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

.PHONY: test
test: bin/assembler_parser_test
	./bin/assembler_parser_test

assembler_sources := $(filter-out $(wildcard assembler/*_test.c),$(wildcard assembler/*.c))
assembler_headers := $(wildcard assembler/*.h)

bin/assembler: $(assembler_sources) $(assembler_headers)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(assembler_sources)

bin/assembler_parser_test: assembler/parser_test.c assembler/parser.c assembler/parser.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ assembler/parser_test.c

.PHONY: clean
clean:
	git clean -xfd
