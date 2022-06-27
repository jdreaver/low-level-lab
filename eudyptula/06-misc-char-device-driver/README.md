# Task 6

## This is Task 06 of the Eudyptula Challenge

Nice job with the module loading macros, those are tricky, but a very
valuable skill to know about, especially when running across them in
real kernel code.

Speaking of real kernel code, let's write some!

The task this time is this:
  - Take the kernel module you wrote for task 01, and modify it to be a
    misc char device driver.  The misc interface is a very simple way to
    be able to create a character device, without having to worry about
    all of the sysfs and character device registration mess.  And what a
    mess it is, so stick to the simple interfaces wherever possible.
  - The misc device should be created with a dynamic minor number, no
    need running off and trying to reserve a real minor number for your
    test module, that would be crazy.
  - The misc device should implement the read and write functions.
  - The misc device node should show up in /dev/eudyptula.
  - When the character device node is read from, your assigned id is
    returned to the caller.
  - When the character device node is written to, the data sent to the
    kernel needs to be checked.  If it matches your assigned id, then
    return a correct write return value.  If the value does not match
    your assigned id, return the "invalid value" error value.
  - The misc device should be registered when your module is loaded, and
    unregistered when it is unloaded.
  - Provide some "proof" this all works properly.

As you will be putting your id into the kernel module, of course you
haven't forgotten it, but just to be safe, it's "7c1caf2f50d1".

## Solution proof

- `make`

```
$ modinfo eudyptula.ko
filename:       /home/david/git/low-level-lab/eudyptula/06-misc-char-device-driver/eudyptula.ko
version:        0.1
description:    Simple misc char device driver for eudyptula task 6
author:         David Reaver
license:        GPL
srcversion:     582825C13627E38790B8B0C
depends:
retpoline:      Y
name:           eudyptula
vermagic:       5.15.49 SMP mod_unload
```

```
$ sudo insmod eudyptula.ko
# from journalctl -k
Jun 27 19:29:45 arnor kernel: Unloaded eudyptula module.
Jun 27 19:29:46 arnor kernel: Loading eudyptula module. Minor number is 255
```

```
$ sudo cat /dev/eudyptula
7c1caf2f50d1⏎
```

```
root@arnor /h/d/g/l/e/06-misc-char-device-driver# echo -n 7c1caf2f50d1 > /dev/eudyptula
root@arnor /h/d/g/l/e/06-misc-char-device-driver# echo $status
0
root@arnor /h/d/g/l/e/06-misc-char-device-driver# echo hello > /dev/eudyptula
write: Invalid argument
```
