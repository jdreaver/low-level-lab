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

Ran in QEMU. I also ran `dmesg -D` to stop kernel messages from appearing in my
console, so the thread print didn't interfere with my commands.

```
[root@nixos:~]# insmod /shared/wee_wait.ko

[root@nixos:~]# ps -ef | grep eudyptula
root         565       2  0 14:40 ?        00:00:00 [eudyptula]
root         569     426  0 14:41 ttyS0    00:00:00 grep eudyptula

[root@nixos:~]# rmmod wee_wait

[root@nixos:~]# dmesg | tail -19
[   76.051593] Loading wee_wait. Minor number is 255
[   76.051623] device: 'wee_wait': device_add
[   76.051633] PM: Adding info for No Bus:wee_wait
[   76.052722] in kernel thread with PID: 565
[   77.063819] in kernel thread with PID: 565
[   78.088080] in kernel thread with PID: 565
[   79.112063] in kernel thread with PID: 565
[   80.135828] in kernel thread with PID: 565
[   81.159829] in kernel thread with PID: 565
[   82.184095] in kernel thread with PID: 565
[   83.208090] in kernel thread with PID: 565
[   84.231805] in kernel thread with PID: 565
[   85.256085] in kernel thread with PID: 565
[   86.280114] in kernel thread with PID: 565
[   87.304082] in kernel thread with PID: 565
[   88.003407] Unloaded wee_wait.
[   88.003411] device: 'wee_wait': device_unregister
[   88.003434] PM: Removing info for No Bus:wee_wait
[   88.003644] device: 'wee_wait': device_create_release
```
