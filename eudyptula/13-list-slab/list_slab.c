#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Reaver");
MODULE_DESCRIPTION("Simple linked list kernel module");
MODULE_VERSION("0.1");

#define NAME_LEN 20

struct identity {
	char name[NAME_LEN];
	int id;
	bool busy;
	struct list_head list;
};

/*
  This constructor is here just so this slab doesn't get merged with others, and
  we can see it in /proc/slabinfo. See
  https://stackoverflow.com/questions/24858424/unable-to-find-new-object-create-with-kmem-cache-create-in-proc-slabinfo
 */
static void identity_constructor(void *addr)
{
	memset(addr, 0, sizeof(struct identity));
}

static LIST_HEAD(identity_list);

static struct kmem_cache *identity_cache;

static int identity_create(char *name, int id)
{
	struct identity *new_ident;

	// TODO: Look for id in existing list and fail if exists

	new_ident = kmem_cache_alloc(identity_cache, GFP_KERNEL);
	if (new_ident == NULL) {
		return -EINVAL;
	}

	strscpy(new_ident->name, name, NAME_LEN);
	new_ident->id = id;
	new_ident->busy = false;
	list_add(&(new_ident->list), &identity_list);

	return 0;
}

static struct identity *identity_find(int id)
{
	struct identity *identity;
	list_for_each_entry(identity, &identity_list, list) {
		if (identity->id == id) {
			return identity;
		}
	}
	return NULL;
}


static void identity_destroy(int id)
{
	struct identity *identity = identity_find(id);
	if (identity != NULL) {
		list_del(&(identity->list));
		kmem_cache_free(identity_cache, identity);
	}

}

static int __init linked_list_start(void)
{
	struct identity *temp;

	pr_info("Loading %s\n", THIS_MODULE->name);

	identity_cache = kmem_cache_create("identity", sizeof(struct identity), 0, 0, identity_constructor);
	if (identity_cache == NULL)
		return -ENOMEM;

	if ((identity_create("Alice", 1)) != 0) {
		return -EINVAL;
	}
	if ((identity_create("Bob", 2)) != 0) {
		return -EINVAL;
	}
	if ((identity_create("David", 3)) != 0) {
		return -EINVAL;
	}
	if ((identity_create("Gena", 10)) != 0) {
		return -EINVAL;
	}

	temp = identity_find(3);
	if (temp == NULL) {
		pr_crit("Failed to find id 3\n");
		return -EINVAL;
	}
	pr_info("id 3 = %s\n", temp->name);

	temp = identity_find(42);
	if (temp == NULL)
		pr_info("id 42 not found\n");

	return 0;
}

static void __exit linked_list_end(void)
{
	struct identity *temp;

	pr_info("Unloading %s\n", THIS_MODULE->name);

        identity_destroy(2);
        identity_destroy(1);
        identity_destroy(10);
        identity_destroy(42);
        identity_destroy(3);

	temp = identity_find(3);
	if (temp == NULL)
		pr_info("id = 3 is gone now\n");

	pr_info("list_empty() == %d\n", list_empty(&identity_list));

	if (identity_cache)
		kmem_cache_destroy(identity_cache);
}

module_init(linked_list_start);
module_exit(linked_list_end);
