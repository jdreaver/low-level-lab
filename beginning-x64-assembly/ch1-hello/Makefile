.PHONY: all
all: hello hello-gas

# N.B. Using gcc for linking (as of gcc version 11.3.0 in May 2022 on NixOS)
# without -nostartfiles `list` not quite working; you get
# `../sysdeps/x86_64/start.S: No such file or directory.`
hello: hello.o
	gcc -o hello -nostartfiles --entry main hello.o

hello.o: hello.asm
	nasm -f elf64 -g -F dwarf hello.asm -l hello.lst


hello-gas: hello-gas.o
	gcc -o hello-gas -nostartfiles --entry main hello-gas.o -ggdb

hello-gas.o: hello-gas.S
	gcc -c hello-gas.S -o hello-gas.o -ggdb


.PHONY: clean
clean:
	rm -f hello hello.o hello-gas hello-gas.o
