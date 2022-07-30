CROSS_COMPILE ?= aarch64-unknown-linux-gnu-
CC = $(CROSS_COMPILE)gcc
READELF = $(CROSS_COMPILE)readelf

CFLAGS += -g3 -ggdb3 -DDEBUG -O0

.PHONY: all
all: hello
	file hello
	$(READELF) -a hello

# Cross-compile using toolchain
hello: hello.c
	$(CC) $(CFLAGS) -static -o $@ $<

.PHONY: clean
clean:
	rm -f hello
