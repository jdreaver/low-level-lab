#include <linux/module.h>     /* Needed by all modules */
#include <linux/kernel.h>     /* Needed for KERN_INFO */
#include <linux/init.h>       /* Needed for the macros */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Reaver");
MODULE_DESCRIPTION("Module to test printk log levels.");
MODULE_VERSION("0.1");

static int __init printk_loglevel_start(void)
{
	pr_emerg("Hello, world @ log-level KERN_EMERG   [0]\n");
	pr_alert("Hello, world @ log-level KERN_ALERT   [1]\n");
	pr_crit("Hello, world @ log-level KERN_CRIT    [2]\n");
	pr_err("Hello, world @ log-level KERN_ERR     [3]\n");
	pr_warn("Hello, world @ log-level KERN_WARNING [4]\n");
	pr_notice("Hello, world @ log-level KERN_NOTICE  [5]\n");
	pr_info("Hello, world @ log-level KERN_INFO    [6]\n");
	pr_debug("Hello, world @ log-level KERN_DEBUG   [7]\n");
	pr_devel("Hello, world via the pr_devel() macro"
		" (eff @KERN_DEBUG) [7]\n");

	return 0;
}

static void __exit printk_loglevel_end(void)
{
	pr_info("Goodbye, world @ log-level KERN_INFO    [6]\n");
}

module_init(printk_loglevel_start);
module_exit(printk_loglevel_end);
