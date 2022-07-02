# Task 13

## This is Task 13 of the Eudyptula Challenge

Weren't those lists fun to play with?  You should get used to them, they
are used all over the kernel in lots of different places.

Now that we are allocating a structure that we want to use a lot of, we
might want to start caring about the speed of the allocation, and not
have to worry about the creation of those objects from the "general"
memory pools of the kernel.

This task is to take the code written in task 12, and cause all memory
allocated from the 'struct identity' to come from a private slab cache
just for the fun of it.

Instead of using kmalloc() and kfree() in the module, use
kmem_cache_alloc() and kmem_cache_free() instead.  Of course this means
you will have to initialize your memory cache properly when the module
starts up.  Don't forget to do that.  You are free to name your memory
cache whatever you wish, but it should show up in the /proc/slabinfo
file.

Don't send the full module for this task, only a patch with the diff
from task 12 showing the lines changed.  This means you will have to
keep a copy of your 12 task results somewhere to make sure you don't
overwrite them.

Also show the output of /proc/slabinfo with your module loaded.


## Proof

Patch (from git diff --no-index -- ../12-linked-list/linked_list.c list_slab.c > diff.patch)

```patch
diff --git a/../12-linked-list/linked_list.c b/list_slab.c
index cb604a0..a22d3a3 100644
--- a/../12-linked-list/linked_list.c
+++ b/list_slab.c
@@ -20,13 +20,15 @@ struct identity {

 static LIST_HEAD(identity_list);

+static struct kmem_cache *identity_cache;
+
 static int identity_create(char *name, int id)
 {
        struct identity *new_ident;

        // TODO: Look for id in existing list and fail if exists

-       new_ident = kmalloc(sizeof(*new_ident), GFP_KERNEL);
+       new_ident = kmem_cache_alloc(identity_cache, GFP_KERNEL);
        if (new_ident == NULL) {
                return -EINVAL;
        }
@@ -56,7 +58,7 @@ static void identity_destroy(int id)
        struct identity *identity = identity_find(id);
        if (identity != NULL) {
                list_del(&(identity->list));
-               kfree(identity);
+               kmem_cache_free(identity_cache, identity);
        }

 }
@@ -67,6 +69,10 @@ static int __init linked_list_start(void)

        pr_info("Loading %s\n", THIS_MODULE->name);

+       identity_cache = kmem_cache_create("identity", sizeof(struct identity), 0, 0, NULL);
+       if (identity_cache == NULL)
+               return -ENOMEM;
+
        if ((identity_create("Alice", 1)) != 0) {
                return -EINVAL;
        }
@@ -108,7 +114,9 @@ static int __init linked_list_start(void)

 static void __exit linked_list_end(void)
 {
-       printk(KERN_INFO "Unloading %s\n", THIS_MODULE->name);
+       pr_info("Unloading %s\n", THIS_MODULE->name);
+       if (identity_cache)
+               kmem_cache_destroy(identity_cache);
 }

 module_init(linked_list_start);
```

Tested in QEMU

```
[root@nixos:~]# insmod /mnt/list_slab.ko
[   83.791161] Loading list_slab
[   83.791280] id 3 = David
[   83.791371] id 42 not found
[   83.791471] id = 3 is gone now
[   83.791579] list_empty() == 1

[root@nixos:~]# rmmod list_slab
[   84.878937] Unloading list_slab
```
