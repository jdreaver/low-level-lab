obj-m = hello.o

# NixOS things. Nice article on this:
# https://blog.prag.dev/building-kernel-modules-on-nixos
#modules_dir=/run/current-system/kernel-modules/lib/modules
linux_modules=$(shell nix build nixpkgs#linux.dev --print-out-paths --no-link)
modules_build_dir=$(wildcard $(linux_modules)/lib/modules/*/build)

# N.B. On a standard Linux system this would be:
# modules_build_dir=/lib/modules/$(shell uname -r)/build/

all:
	make -C $(modules_build_dir) M=$(PWD) modules
clean:
	make -C $(modules_build_dir) M=$(PWD) clean
