.PHONY: all
all: os.img

.PHONY: run
run: os.img
	qemu-system-x86_64 -drive format=raw,file=$<

os.img: boot/boot.bin kernel/kernel.bin
	cp boot/boot.bin ./$@
	cat kernel/kernel.bin >> $@
	@fsize=$$(wc -c < os.img) && export sectors=$$(( $$fsize / 512 )) && echo "ALERT: Adjust boot sector to load $$sectors sectors"

# N.B. It is important that entry.o comes first! We want to run from _start.
# Also, note the use of ld.lld, not ld.
kernel/kernel.bin: kernel/entry.o kernel/kernel.o
	ld.lld -o $@ -Ttext 0x8200 --oformat binary $^

kernel/kernel.o: kernel/kernel.c
	clang -c -ffreestanding -target x86_64-none-elf -o $@ $<

kernel/entry.o: kernel/entry.asm
	nasm -f elf64 -o $@ $<

boot/boot.bin: boot/boot.asm
	nasm -f bin -o $@ $<

.PHONY: clean
clean:
	rm -f **/*.o **/*.bin **/*.img
