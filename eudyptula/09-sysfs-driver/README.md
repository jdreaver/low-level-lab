# Task 9

## This is Task 09 of the Eudyptula Challenge

Nice job with debugfs, that is a handy thing to remember when wanting to
print out some debugging information.  Never use /proc/ that is only for
processes, use debugfs instead.

Along with debugfs, sysfs is a common place to put information that
needs to move from the user to the kernel.  So let us focus on sysfs for
this task.

The task this time:

  - Take the code you wrote in task 08, and move it to sysfs.  Put the
    "eudyptula" directory under the /sys/kernel/ location in sysfs.
  - Provide some "proof" this works.

That's it!  Simple, right?  No, you are right, it's more complex, sysfs
is complicated.  I'd recommend reading Documentation/kobject.txt as a
primer on how to use kobjects and sysfs.

Feel free to ask for hints and help with this one if you have questions
by sending in code to review if you get stuck, many people have dropped
out in the challenge at this point in time, so don't feel bad about
asking, we don't want to see you go away just because sysfs is so damn
complicated.

## Solution proof

I build this with `make MODULES_BUILD_DIR=~/git/kernel-dev/linux` and ran this
in a QEMU VM with the serial console, so dmesg output is inline.

```
[root@nixos:~]# modinfo /mnt/eudyptula_sysfs.ko
filename:       /mnt/eudyptula_sysfs.ko
version:        0.1
description:    sysfs device driver for eudyptula task 8
author:         David Reaver
license:        GPL
srcversion:     FBFE1AE32AF6DB9E43FE9DE
depends:
retpoline:      Y
name:           eudyptula_sysfs
vermagic:       5.19.0-rc3-00336-g0840a7914caa SMP preempt mod_unload
```

```
[root@nixos:~]# insmod /mnt/eudyptula_sysfs.ko
[   20.051807] eudyptula_sysfs: loading out-of-tree module taints kernel.
[   20.052193] Loading module eudyptula_sysfs```

```

```
[root@nixos:~]# echo hello > /sys/kernel/eudyptula/id
[   62.762277] in eudyptula_sysfs id_write. count: 6
-bash: echo: write error: Invalid argument

[root@nixos:~]# echo -n 7c1caf2f50d1 > /sys/kernel/eudyptula/id
[   98.154329] in eudyptula_sysfs id_write. count: 12
```

```
[root@nixos:~]# cat /sys/kernel/eudyptula/jiffies
[  160.655781] in eudyptula_sysfs jiffies_read, jiffies = 4294827912
4294827912
```

```
[root@nixos:~]# echo hello > /sys/kernel/eudyptula/foo
[   24.671574] in eudyptula_sysfs foo_write. count: 6

[root@nixos:~]# cat /sys/kernel/eudyptula/foo
hello

[root@nixos:~]# echo goodbye > /sys/kernel/eudyptula/foo
[   29.263484] in eudyptula_sysfs foo_read
[   35.865557] in eudyptula_sysfs foo_write. count: 8

[root@nixos:~]# cat /sys/kernel/eudyptula/foo
goodbye
```

```
[root@nixos:~]# rmmod eudyptula_sysfs
[   42.680357] in eudyptula_sysfs foo_read
[   62.856779] Unloaded eudyptula module.
```
