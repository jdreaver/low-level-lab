.PHONY: all
all: alive alive2 hello4

# N.B. Using gcc for linking (as of gcc version 11.3.0 in May 2022 on NixOS)
# without -nostartfiles `list` not quite working; you get
# `../sysdeps/x86_64/start.S: No such file or directory.`
alive: alive.o
	gcc -o $@ -nostartfiles --entry main $?

alive.o: alive.asm
	nasm -f elf64 -g -F dwarf $? -l alive.lst

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
