.PHONY: all
all: hello hello-gas hello-gas-intel

hello: hello.o
	ld -o $@ $?

# N.B. We use yasm instead of nasm because nasm 2.15.05 (Aug 28 2020) and gdb
# 12.1 (May 2022) don't play nice together. There doesn't seem to be any source
# location information included in the resulting binary. Specifically, we have
# to use "ni" instead of "n" for stepping, and any command that tries to inspect
# the source (like layout src, or TUI mode), says "[No Source Available]".
hello.o: hello.asm
	yasm -f elf64 -g dwarf2 $? -l hello.lst

hello-gas: hello-gas.o
	gcc -o $@ -nostartfiles --entry main $? -ggdb

hello-gas.o: hello-gas.S
	gcc -c $? -o $@ -ggdb


hello-gas-intel: hello-gas-intel.o
	gcc -o $@ -nostartfiles --entry main $? -ggdb

hello-gas-intel.o: hello-gas-intel.S
	gcc -c $? -o $@ -ggdb


.PHONY: clean
clean:
	rm -f hello hello.o hello-gas hello-gas.o hello-gas-intel hello-gas-intel.o
