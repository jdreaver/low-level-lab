# OSDev Bare Bones kernel tutorial

Following the tutorial at <https://wiki.osdev.org/Bare_Bones>. This builds a
bare bones 32 bit x86 kernel and runs it with QEMU.

## Usage

You can either run an ISO with GRUB, or you can run the kernel bin directly
because QEMU supports launching multiboot-compatible kernels directly.

```
$ make run-bin
$ make run-iso
```

You should see a QEMU window pop up that says "Hello, kernel World!".
