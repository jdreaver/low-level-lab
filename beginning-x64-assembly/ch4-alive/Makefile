.PHONY: all
all: alive hello4

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

.PHONY: clean
clean:
	rm -f alive alive.o hello4 hello4.o
