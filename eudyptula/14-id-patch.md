# Task 14

## This is Task 14 of the Eudyptula Challenge

Now that you have the basics of lists, and we glossed over the custom
allocators (the first cut at that task was much harder, you got off
easy), it's time to move on to something a bit more old-school: tasks.

For this task:
  - Add a new field to the core kernel task structure called, wait for
    it, "id".
  - When the task is created, set the id to your id.  Imaginative, I
    know.  You try writing these tasks.
  - Add a new proc file for every task called, "id", located in the
    /proc/${PID}/ directory for that task.
  - When the proc file is read from, have it print out the value of
    your id, and then increment it by one, allowing different tasks to
    have different values for the "id" file over time as they are read
    from.
  - Provide some "proof" it all works properly.

As you are touching files all over the kernel tree, a patch is the
required result to be sent in here.  Please specify which kernel version
you make this patch against, to give my virtual machines a chance to
figure out how to apply it.

Also provide some kind of proof that you tested the patch.

And, in case you happened to forget it, your id is "7c1caf2f50d1".

## Proof

```patch
From 8f74052c92c2b81b04dee1c81b2d20525cd3100c Mon Sep 17 00:00:00 2001
From: David Reaver <me@davidreaver.com>
Date: Sat, 2 Jul 2022 12:11:37 -0700
Subject: [PATCH] proc: base: Add /proc/<pid>/id with incrementing eudyptula id

This patch adds a proc file for each process that prints my eudyptula
ID, incremented by 1 every time the file is read. For example:

    [root@nixos:~]# printf "%x" $(cat /proc/1/id)
    7c1caf2f50d1
    [root@nixos:~]# printf "%x" $(cat /proc/1/id)
    7c1caf2f50d2
    [root@nixos:~]# printf "%x" $(cat /proc/12/id)
    7c1caf2f50d1
    [root@nixos:~]# printf "%x" $(cat /proc/12/id)
    7c1caf2f50d2
    [root@nixos:~]# printf "%x" $(cat /proc/1/id)
    7c1caf2f50d3

Signed-off-by: David Reaver <me@davidreaver.com>
---
 fs/proc/base.c        | 9 +++++++++
 include/linux/sched.h | 2 ++
 kernel/fork.c         | 2 ++
 3 files changed, 13 insertions(+)

diff --git a/fs/proc/base.c b/fs/proc/base.c
index 8dfa36a99c74..d9f3ee284f69 100644
--- a/fs/proc/base.c
+++ b/fs/proc/base.c
@@ -3185,6 +3185,13 @@ static int proc_stack_depth(struct seq_file *m, struct pid_namespace *ns,
 }
 #endif /* CONFIG_STACKLEAK_METRICS */

+static int proc_eudyptula_id(struct seq_file *m, struct pid_namespace *ns, struct pid *pid, struct task_struct *task)
+{
+	seq_printf(m, "%ld\n", task->id);
+	task->id++;
+	return 0;
+}
+
 /*
  * Thread groups
  */
@@ -3304,6 +3311,8 @@ static const struct pid_entry tgid_base_stuff[] = {
 #ifdef CONFIG_KSM
 	ONE("ksm_merging_pages",  S_IRUSR, proc_pid_ksm_merging_pages),
 #endif
+
+	ONE("id", S_IRUSR, proc_eudyptula_id),
 };

 static int proc_tgid_base_readdir(struct file *file, struct dir_context *ctx)
diff --git a/include/linux/sched.h b/include/linux/sched.h
index c46f3a63b758..fdebbb792aef 100644
--- a/include/linux/sched.h
+++ b/include/linux/sched.h
@@ -1500,6 +1500,8 @@ struct task_struct {
 	struct callback_head		l1d_flush_kill;
 #endif

+	long id;
+
 	/*
 	 * New fields for task_struct should be added above here, so that
 	 * they are included in the randomized portion of task_struct.
diff --git a/kernel/fork.c b/kernel/fork.c
index 9d44f2d46c69..d6960d5ea725 100644
--- a/kernel/fork.c
+++ b/kernel/fork.c
@@ -1049,6 +1049,8 @@ static struct task_struct *dup_task_struct(struct task_struct *orig, int node)
 	tsk->reported_split_lock = 0;
 #endif

+	tsk->id = 0x7c1caf2f50d1;
+
 	return tsk;

 free_stack:
```

Running in QEMU:

```
[root@nixos:~]# printf "%x" $(cat /proc/1/id)
7c1caf2f50d1
[root@nixos:~]# printf "%x" $(cat /proc/1/id)
7c1caf2f50d2
[root@nixos:~]# printf "%x" $(cat /proc/1/id)
7c1caf2f50d3
[root@nixos:~]# printf "%x" $(cat /proc/1/id)
7c1caf2f50d4
[root@nixos:~]# printf "%x" $(cat /proc/1/id)
7c1caf2f50d5
[root@nixos:~]# printf "%x" $(cat /proc/12/id)
7c1caf2f50d1
[root@nixos:~]# printf "%x" $(cat /proc/12/id)
7c1caf2f50d2
[root@nixos:~]# printf "%x" $(cat /proc/12/id)
7c1caf2f50d3
[root@nixos:~]# printf "%x" $(cat /proc/1/id)
7c1caf2f50d6
```
