# Task 15

## This is Task 15 of the Eudyptula Challenge

That process task turned out to not be all that complex, but digging
through the core kernel was a tough task, nice work with that.

Speaking of "core kernel" tasks, let's do another one.  It's one of the
most common undergraduate tasks there is: create a new syscall!
Yeah, loads of fun, but it's good to know the basics of how to do this,
and, how to call it from userspace.

For this task:
  - Add a new syscall to the kernel called "sys_eudyptula", so this is
    all going to be changes to the kernel tree itself, no stand-alone
    module needed for this task (unless you want to do it that way
    without hacking around the syscall table, if so, bonus points for
    you...)
  - The syscall number needs to be the next syscall number for the
    architecture you test it on (some of you all are doing this on ARM
    systems, showoffs, and syscall numbers are not the same across all
    architectures).  Document the arch you are using and why you picked
    this number in the module.
  - The syscall should take two parameters: int high_id, int low_id.
  - The syscall will take the two values, mush them together into one
    64bit value (low_id being the lower 32bits of the id, high_id being
    the upper 32bits of the id).
  - If the id value matches your id (which of course you know as
    "7c1caf2f50d1", then the syscall returns success.  Otherwise it
    returns a return code signifying an invalid value was passed to it.
  - Write a userspace C program that calls the syscall and properly
    exercises it (valid and invalid calls need to be made).
  - "Proof" of running the code needs to be provided.

So you need to send in a .c userspace program, a kernel patch, and
"proof" that it all works, as a response.

## Proof

Ran in QEMU with this directory mounted.

```
[root@nixos:~]# /mnt/test_eudyptula
sys_eudyptula(1, 1) = 18446744073709551615
sys_eudyptula(0x7c1c,0xaf2f50d1) = 0
```

```patch
From 21e351dc72b94d1f0b371fcf40c495c0b3df0ffc Mon Sep 17 00:00:00 2001
From: David Reaver <me@davidreaver.com>
Date: Sat, 2 Jul 2022 15:41:49 -0700
Subject: [PATCH] sys: Add eudyptula syscall

The eudyptula syscall returns success if the two passed values combine
to my eudyptula ID.

Signed-off-by: David Reaver <me@davidreaver.com>
---
 arch/x86/entry/syscalls/syscall_64.tbl |  1 +
 include/linux/syscalls.h               |  2 ++
 kernel/sys.c                           | 10 ++++++++++
 3 files changed, 13 insertions(+)

diff --git a/arch/x86/entry/syscalls/syscall_64.tbl b/arch/x86/entry/syscalls/syscall_64.tbl
index c84d12608cd2..8e255f525ee8 100644
--- a/arch/x86/entry/syscalls/syscall_64.tbl
+++ b/arch/x86/entry/syscalls/syscall_64.tbl
@@ -372,6 +372,7 @@
 448	common	process_mrelease	sys_process_mrelease
 449	common	futex_waitv		sys_futex_waitv
 450	common	set_mempolicy_home_node	sys_set_mempolicy_home_node
+451	64	eudyptula		sys_eudyptula

 #
 # Due to a historical design error, certain syscalls are numbered differently
diff --git a/include/linux/syscalls.h b/include/linux/syscalls.h
index a34b0f9a9972..a03425641e5f 100644
--- a/include/linux/syscalls.h
+++ b/include/linux/syscalls.h
@@ -1276,6 +1276,8 @@ asmlinkage long sys_old_mmap(struct mmap_arg_struct __user *arg);
  */
 asmlinkage long sys_ni_syscall(void);

+asmlinkage long sys_eudyptula(unsigned int hi_id, unsigned int lo_id);
+
 #endif /* CONFIG_ARCH_HAS_SYSCALL_WRAPPER */


diff --git a/kernel/sys.c b/kernel/sys.c
index b911fa6d81ab..6d008cdfb996 100644
--- a/kernel/sys.c
+++ b/kernel/sys.c
@@ -2788,3 +2788,13 @@ COMPAT_SYSCALL_DEFINE1(sysinfo, struct compat_sysinfo __user *, info)
 	return 0;
 }
 #endif /* CONFIG_COMPAT */
+
+#define eudyptula_id 0x7c1caf2f50d1
+
+SYSCALL_DEFINE2(eudyptula, unsigned int, high_id, unsigned int, low_id)
+{
+	u64 val = low_id | ((u64)high_id << 32);
+	if (val != eudyptula_id)
+		return -EINVAL;
+	return 0;
+}
```
