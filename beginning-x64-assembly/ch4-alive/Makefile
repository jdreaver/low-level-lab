.PHONY: all
all: alive alive2 hello4

alive: alive.o
	ld -o $@ $?

# N.B. We use yasm instead of nasm because nasm 2.15.05 (Aug 28 2020) and gdb
# 12.1 (May 2022) don't play nice together. There doesn't seem to be any source
# location information included in the resulting binary. Specifically, we have
# to use "ni" instead of "n" for stepping, and any command that tries to inspect
# the source (like layout src, or TUI mode), says "[No Source Available]".
alive.o: alive.asm
	yasm -f elf64 -g dwarf2 $? -l alive.lst

hello4: hello4.o
	gcc -o $@ -nostartfiles --entry main $?

hello4.o: hello4.asm
	nasm -f elf64 -g -F dwarf $? -l hello4.lst

alive2: alive2.o
	gcc -o $@ -nostartfiles --entry main $?

alive2.o: alive2.S
	gcc -c $? -o $@ -ggdb

.PHONY: clean
clean:
	rm -f alive alive.o alive2 alive2.o hello4 hello4.o
