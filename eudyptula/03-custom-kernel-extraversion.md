# Task 2: Custom kernel with extra version

## This is Task 03 of the Eudyptula Challenge

Now that you have your custom kernel up and running, it's time to modify
it!

The tasks for this round is:
  - take the kernel git tree from Task 02 and modify the Makefile to
    and modify the EXTRAVERSION field.  Do this in a way that the
    running kernel (after modifying the Makefile, rebuilding, and
    rebooting) has the characters "-eudyptula" in the version string.
  - show proof of booting this kernel.  Extra cookies for you by
    providing creative examples, especially if done in intrepretive
    dance at your local pub.
  - Send a patch that shows the Makefile modified.  Do this in a manner
    that would be acceptable for merging in the kernel source tree.
    (Hint, read the file Documentation/SubmittingPatches and follow the
    steps there.)

Remember to use your ID assigned to you in the Subject: line when
responding to this task, so that I can figure out who to attribute it
to.

If you forgot, your id is "7c1caf2f50d1".  Surely I don't need to keep
saying this right?  I know, _you_ wouldn't forget, but someone else, of
course they would, so I'll just leave it here for those "others".

## Solution

- Modified kernel `Makefile` w/ `EXTRAVERSION = -rc3-7c1caf2f50d1`
- Booted/test with:

```
root@arnor:~# uname -r
5.19.0-rc3-7c1caf2f50d1-00336-g0840a7914caa-dirty
```

Here is my patch:

```patch
From 1deee6e1a7a6f1fd842c8c5265f45a0a4211a1e4 Mon Sep 17 00:00:00 2001
From: David Reaver <me@davidreaver.com>
Date: Sun, 26 Jun 2022 09:35:32 -0700
Subject: [PATCH] Makefile: add eudalypta challenge ID to EXTRAVERSION

This is part of the Eudalypta challenge task 3, where I am tasked with
adding my custom ID to the EXTRAVERSION field of the Linux Makefile.
Tested with:

  # uname -r
  5.19.0-rc3-7c1caf2f50d1-00336-g0840a7914caa-dirty

Signed-off-by: David Reaver <me@davidreaver.com>
---
 Makefile | 2 +-
 1 file changed, 1 insertion(+), 1 deletion(-)

diff --git a/Makefile b/Makefile
index 513c1fbf7888..7885270dffa2 100644
--- a/Makefile
+++ b/Makefile
@@ -2,7 +2,7 @@
 VERSION = 5
 PATCHLEVEL = 19
 SUBLEVEL = 0
-EXTRAVERSION = -rc3
+EXTRAVERSION = -rc3-7c1caf2f50d1
 NAME = Superb Owl

 # *DOCUMENTATION*
```
