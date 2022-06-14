# N.B. Using gcc for linking (as of gcc version 11.3.0 in May 2022 on NixOS)
# results in debug symbols not quite working. If you use gdb on the resulting
# binary and try to run `list`, you get `../sysdeps/x86_64/start.S: No such file
# or directory.`
#
# Using ld directly seems to not have this problem.
hello: hello.o
	ld -o hello --entry main hello.o

hello.o: hello.asm
	nasm -f elf64 -g -F dwarf hello.asm -l hello.lst

.PHONY: clean
clean:
	rm -f hello hello.o
