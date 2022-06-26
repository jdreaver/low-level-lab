# Task 1

## This is Task 01 of the Eudyptula Challenge

Write a Linux kernel module, and stand-alone Makefile, that when loaded
prints to the kernel debug log level, "Hello World!"  Be sure to make
the module be able to be unloaded as well.

The Makefile should build the kernel module against the source for the
currently running kernel, or, use an environment variable to specify
what kernel tree to build it against.

Please show proof of this module being built, and running, in your
kernel.  What this proof is is up to you, I'm sure you can come up with
something.  Also be sure to send the kernel module you wrote, along with
the Makefile you created to build the module.

Remember to use your ID assigned to you in the Subject: line when
responding to this task, so that I can figure out who to attribute it
to.  You can just respond to the task with the answers and all should be
fine.

If you forgot, your id is "7c1caf2f50d1".  But of course you have not
forgotten that yet, you are better than that.

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
