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
	$(CC) $(CFLAGS) -o $@ $(assembler_sources)


unity_headers := $(wildcard unity/*.h)
unity_sources := $(wildcard unity/*.c)
UNITY_O=unity/unity.o

$(UNITY_O): $(unity_headers) $(unity_sources)
	$(CC) $(CFLAGS) -c -o $@ $(unity_sources)

bin/assembler_parser_test: assembler/parser_test.c assembler/parser.c assembler/parser.h $(unity_headers) $(UNITY_O)
	$(CC) $(CFLAGS) -o $@ assembler/parser_test.c -Iunity/ $(UNITY_O) -DTEST

.PHONY: clean
clean:
	git clean -xfd
