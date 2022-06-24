obj-m = printk_loglevel.o

# NixOS things. Nice article on this:
# https://blog.prag.dev/building-kernel-modules-on-nixos
#modules_dir=/run/current-system/kernel-modules/lib/modules
linux_modules=$(shell nix build nixpkgs#linux.dev --print-out-paths --no-link)
modules_build_dir=$(wildcard $(linux_modules)/lib/modules/*/build)

# N.B. On a standard Linux system this would be:
# modules_build_dir=/lib/modules/$(shell uname -r)/build/

# Enable the pr_debug() and pr_devel() as well by removing the comment from
# one of the lines below
EXTRA_CFLAGS += -DDEBUG
#CFLAGS_printk_loglvl.o := -DDEBUG

all:
	make -C $(modules_build_dir) M=$(PWD) modules
clean:
	make -C $(modules_build_dir) M=$(PWD) clean
