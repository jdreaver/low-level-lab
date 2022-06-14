# N.B. Using gcc for linking (as of gcc version 11.3.0 in May 2022 on NixOS)
# results in debug symbols not quite working. If you use gdb on the resulting
# binary and try to run `list`, you get `../sysdeps/x86_64/start.S: No such file
# or directory.`
#
# Using ld directly seems to not have this problem.
alive: alive.o
	ld -o alive --entry main alive.o

alive.o: alive.asm
	nasm -f elf64 -g -F dwarf alive.asm -l alive.lst

.PHONY: clean
clean:
	rm -f alive alive.o
