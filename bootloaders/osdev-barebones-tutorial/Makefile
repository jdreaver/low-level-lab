.PHONY: all
all: myos.iso myos.bin

.PHONY: run-iso
run-iso: myos.iso
	qemu-system-i386 -cdrom myos.iso

.PHONY: run-bin
run-bin: myos.bin
	qemu-system-i386 -kernel myos.bin

boot.o: boot.s
	i686-elf-as -o $@ $<

kernel.o: kernel.c
	i686-elf-gcc -c $< -o $@ -std=gnu99 -ffreestanding -O2 -Wall -Wextra

myos.bin: boot.o kernel.o
	i686-elf-gcc -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc
	grub-file --is-x86-multiboot myos.bin

myos.iso: myos.bin grub.cfg
	rm -rf isodir
	mkdir -p isodir/boot/grub
	cp myos.bin isodir/boot/myos.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o $@ isodir

.PHONY: clean
clean:
	rm -f *.o *.bin *.iso
