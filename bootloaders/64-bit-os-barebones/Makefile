.PHONY: all
all: boot.bin

.PHONY: run
run: boot.bin
	qemu-system-x86_64 $<

boot.bin: boot.asm
	nasm -f bin -o $@ $<
