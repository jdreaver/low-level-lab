obj-m = wee_wait.o

# Set MODULES_BUILD_DIR to e.g. a local linux source tree if you want
ifeq ($(MODULES_BUILD_DIR),)
  # NixOS things. Nice article on this:
  # https://blog.prag.dev/building-kernel-modules-on-nixos
  #modules_dir=/run/current-system/kernel-modules/lib/modules
  linux_modules=$(shell nix build nixpkgs#linux.dev --print-out-paths --no-link)
  MODULES_BUILD_DIR=$(wildcard $(linux_modules)/lib/modules/*/build)

  # N.B. On a standard Linux system this would be:
  # MODULES_BUILD_DIR=/lib/modules/$(shell uname -r)/build/
endif

KERNEL_MAKE=make -C $(MODULES_BUILD_DIR) M=$(PWD)

.PHONY: all
all: module compile_commands.json

.PHONY: module
module:
	$(KERNEL_MAKE) modules

compile_commands.json:
	$(KERNEL_MAKE) compile_commands.json

.PHONY: clean
clean:
	$(KERNEL_MAKE) clean
