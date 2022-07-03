# Task 17

## This is Task 17 of the Eudyptula Challenge

Another patch made and sent in.  See, that wasn't so hard.  Keep sending
in kernel patches outside of this challenge, those lazy kernel
developers need all the help they can get in cleaning up their code.

It is time to start putting the different pieces of what we have done in
the past together, into a much larger module, doing more complex things.
Much more like what a "real" kernel module has to do.

Go dig up your code from task 06, the misc char device driver, and make
the following changes:

 - Delete the read function.  You don't need that anymore, so make it a
   write-only misc device and be sure to set the mode of the device to
   be write-only, by anyone.  If you do this right, udev will set up the
   node automatically with the correct permissions.
 - Create a wait queue, name it "wee_wait".
 - In your module init function, create a kernel thread, named of course
   "eudyptula".
 - The thread's main function should not do anything at this point in
   time, except make sure to shutdown if asked to, and wait on the
   "wee_wait" waitqueue.
 - In your module exit function, shut down the kernel thread you started
   up.

Load and unload the module and "prove" that it works properly (that the
thread is created, it can be found in the process list, and that the
device node is created with the correct permission value.)  Send in the
proof and the .c file for the module.

Be sure to keep this code around, as we will be doing more with it next
time.

## Proof

Ran in QEMU.

```
[root@nixos:~]# insmod /shared/wee_wait.ko
[   10.985008] wee_wait: loading out-of-tree module taints kernel.
[   10.985385] Loading wee_wait. Minor number is 255

[root@nixos:~]# [   10.986708] in kernel thread with PID: 557


[root@nixos:~]# ps -ef | grep eudyptula
root         557       2  0 15:01 ?        00:00:00 [eudyptula]
root         559     426  0 15:01 ttyS0    00:00:00 grep eudyptula

[root@nixos:~]# rmmod wee_wait
[   28.676479] Unloaded wee_wait.
```
