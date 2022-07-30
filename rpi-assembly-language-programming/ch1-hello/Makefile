#CROSS_COMPILE ?= aarch64-unknown-linux-gnu-
CROSS_COMPILE ?= armv7l-unknown-linux-gnueabihf-
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)as
LD = $(CROSS_COMPILE)ld
READELF = $(CROSS_COMPILE)readelf

CFLAGS += -g3 -ggdb3 -DDEBUG -O0

.PHONY: all
all: hello-c hello-asm
	file hello-asm
	$(READELF) -a hello-asm

	file hello-c
	$(READELF) -a hello-c

# Cross-compile using toolchain
hello-c: hello-c.o
	$(CC) -static -o $@ $<

hello-c.o: hello.c
	$(CC) $(CFLAGS) -c -o $@ $<

hello-asm: hello-asm.o
	$(LD) -static -o $@ $<

hello-asm.o: hello.S
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -f hello-c hello-asm *.o
