# Task 8

## This is Task 08 of the Eudyptula Challenge

We will come back to the linux-next kernel in a later exercise, so don't
go and delete that directory, you'll want it around.  But enough of
building kernels, let's write more code!

This task is much like the 06 task with the misc device, but this time
we are going to focus on another user/kernel interface, debugfs.  It is
rumored that the creator of debugfs said that there is only one rule for
debugfs use, "There are no rules when using debugfs."  So let's take
them up on that offer and see how to use it.

debugfs should be mounted by your distro in /sys/kernel/debug/, if it
isn't, then you can mount it with the line:
        mount -t debugfs none /sys/kernel/debug/

Make sure it is enabled in your kernel, with the CONFIG_DEBUG_FS option,
you will need it for this task.

The task, in specifics is:

  - Take the kernel module you wrote for task 01, and modify it to be
    create a debugfs subdirectory called "eudyptula".  In that
    directory, create 3 virtual files called "id", "jiffies", and "foo".
  - The file "id" operates just like it did for example 06, use the same
    logic there, the file must be readable and writable by any user.
  - The file "jiffies" is to be read only by any user, and when read,
    should return the current value of the jiffies kernel timer.
  - The file "foo" needs to be writable only by root, but readable by
    anyone.  When writing to it, the value must be stored, up to one
    page of data.  When read, which can be done by any user, the value
    must be returned that is stored it it.  Properly handle the fact
    that someone could be reading from the file while someone else is
    writing to it (oh, a locking hint!)
  - When the module is unloaded, all of the debugfs files are cleaned
    up, and any memory allocated is freed.
  - Provide some "proof" this all works.

Again, you are using your id in the code, so you know what it is by now,
no need to repeat it again.

## Solution proof

- `make`

```
$ modinfo eudyptula_debugfs.ko
filename:       /home/david/git/low-level-lab/eudyptula/08-debugfs-driver/eudyptula_debugfs.ko
version:        0.1
description:    debugfs device driver for eudyptula task 8
author:         David Reaver
license:        GPL
srcversion:     A958626CBDDBB996621450E
depends:
retpoline:      Y
name:           eudyptula_debugfs
vermagic:       5.15.49 SMP mod_unload
```

```
$ sudo insmod eudyptula_debugfs.ko
```

```
david@arnor > sudo bash -c "echo hello > /sys/kernel/debug/eudyptula/id"
bash: line 1: echo: write error: Invalid argument
david@arnor > sudo bash -c "echo -n 7c1caf2f50d1 > /sys/kernel/debug/eudyptula/id"
david@arnor > echo $status
0
```

```
david@arnor > sudo cat /sys/kernel/debug/eudyptula/jiffies
4297584825⏎
```

```
david@arnor > sudo bash -c "echo hello > /sys/kernel/debug/eudyptula/foo"
david@arnor > sudo cat /sys/kernel/debug/eudyptula/foo
hello
david@arnor > sudo bash -c "echo goodbye > /sys/kernel/debug/eudyptula/foo"
david@arnor > sudo cat /sys/kernel/debug/eudyptula/foo
goodbye
```

```
$ sudo rmmod -f eudyptula_debugfs
```

`journalctl -k` output for all of this:

```
Jun 28 06:30:42 arnor kernel: Loading module eudyptula_debugfs
Jun 28 06:31:12 arnor kernel: in eudyptula_debugfs eudyptula_id_read
Jun 28 06:31:32 arnor kernel: in eudyptula_debugfs eudyptula_id_read
Jun 28 06:31:32 arnor kernel: in eudyptula_debugfs eudyptula_id_write. count: 6
Jun 28 06:31:32 arnor kernel: [44B blob data]
Jun 28 06:31:52 arnor kernel: in eudyptula_debugfs eudyptula_id_write. count: 12
Jun 28 06:31:52 arnor kernel: copied message: 7c1caf2f50d1, strncmp() = 0
Jun 28 06:32:18 arnor kernel: in eudyptula_debugfs jiffies_read, jiffies = 4297584825
Jun 28 06:32:18 arnor kernel: in eudyptula_debugfs jiffies_read, jiffies = 4297584825
Jun 28 06:32:36 arnor kernel: in eudyptula_debugfs eudyptula_foo_write. count: 6
Jun 28 06:32:39 arnor kernel: in eudyptula_debugfs eudyptula_foo_read
Jun 28 06:32:47 arnor kernel: in eudyptula_debugfs eudyptula_foo_read
Jun 28 06:32:47 arnor kernel: in eudyptula_debugfs eudyptula_foo_write. count: 8
Jun 28 06:32:48 arnor kernel: in eudyptula_debugfs eudyptula_foo_read
Jun 28 06:33:01 arnor kernel: in eudyptula_debugfs eudyptula_foo_read
Jun 28 06:33:01 arnor kernel: Unloaded eudyptula module.
```
