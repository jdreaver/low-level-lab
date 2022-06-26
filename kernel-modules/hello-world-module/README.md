# Hello World kernel module

This is a trivial Linux kernel module, inspired from
https://www.geeksforgeeks.org/linux-kernel-module-programming-hello-world-program/
and https://www.embeddedrelated.com/showarticle/1274.php

## Usage

1. Compile with `make`
2. Check out `modinfo`
   ```
   modinfo hello.ko
   filename:       /home/david/git/low-level-lab/kernel/hello-world-module/hello.ko
   version:        0.1
   description:    A simple Hello world LKM!
   author:         David Reaver
   license:        GPL
   srcversion:     F9633D2CE1F7587F6285C8B
   depends:
   retpoline:      Y
   name:           hello
   vermagic:       5.15.47 SMP mod_unload
   ```
3. Load the module with `sudo insmod hello.ko`
4. See output with `dmesg | tail`
   ```
   ...
   [48127.169359] Loading hello module...
   [48127.169360] Hello world
   ```
5. Unload module with `sudo rmmod hello.ko`
6. See output with `dmesg | tail`
   ```
   ...
   [48127.169359] Loading hello module...
   [48127.169360] Hello world
   [48153.075459] Goodbye Mr.
   ```
