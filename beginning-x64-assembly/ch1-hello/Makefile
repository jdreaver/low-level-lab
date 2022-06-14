.PHONY: all
all: hello hello-gas hello-gas-intel

# N.B. Using gcc for linking (as of gcc version 11.3.0 in May 2022 on NixOS)
# without -nostartfiles `list` not quite working; you get
# `../sysdeps/x86_64/start.S: No such file or directory.`
hello: hello.o
	gcc -o $@ -nostartfiles --entry main $?

hello.o: hello.asm
	nasm -f elf64 -g -F dwarf $? -l hello.lst


hello-gas: hello-gas.o
	gcc -o $@ -nostartfiles --entry main $? -ggdb

hello-gas.o: hello-gas.S
	gcc -c $? -o hello-gas.o -ggdb


hello-gas-intel: hello-gas-intel.o
	gcc -o $@ -nostartfiles --entry main $? -ggdb

hello-gas-intel.o: hello-gas-intel.S
	gcc -c $? -o hello-gas-intel.o -ggdb


.PHONY: clean
clean:
	rm -f hello hello.o hello-gas hello-gas.o hello-gas-intel hello-gas-intel.o
