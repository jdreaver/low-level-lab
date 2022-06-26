#include <linux/init.h>
#include <linux/cred.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>

#define MODNAME "current_task"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Reaver");
MODULE_DESCRIPTION("Shows information about the task that loads/unloads this module");
MODULE_VERSION("0.1");

static void show_ctx(void)
{
	/* Extract task UID and EID */
	uid_t uid = from_kuid(&init_user_ns, current_uid());
	uid_t euid = from_kuid(&init_user_ns, current_euid());

	pr_info("%s:%s():%d ", MODNAME, __func__, __LINE__);
	if (likely(in_task())) {
		pr_info("%s: in process context ::\n"
			" PID         : %6d\n"
			" TGID        : %6d\n"
			" UID         : %6u\n"
			" EUID        : %6u (%s root)\n"
			" name        : %s\n"
			" current (ptr to our process context's task_struct) :\n"
			"               0x%pK (0x%px)\n"
			" stack start : 0x%pK (0x%px)\n", MODNAME,
			/* always better to use the helper methods provided */
			task_pid_nr(current), task_tgid_nr(current),
			/* ... rather than the 'usual' direct lookups:
			 * current->pid, current->tgid,
			 */
			uid, euid,
			(euid == 0 ? "have" : "don't have"),
			current->comm,
			current, current,
			current->stack, current->stack);
	} else
		pr_alert("%s: in interrupt context [Should NOT Happen here!]\n", MODNAME);
}

static int __init hello_start(void)
{
	pr_info("%s: inserted\n", MODNAME);
	pr_info(" sizof(struct task_struct) = %zd\n", sizeof(struct task_struct));
	show_ctx();
	return 0;
}

static void __exit hello_end(void)
{
	pr_info("%s: removed\n", MODNAME);
	show_ctx();
}

module_init(hello_start);
module_exit(hello_end);
