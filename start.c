#include "syscall.h"

#define DRIVER_AUTHOR "kindtime"
#define DRIVER_DESC "kindtime"

static int __init hello_init(void)
{
	pr_info("hello from pr info\n");
	printk(KERN_ALERT "return val %d\n", variable);

	original_call = sys_call_table[__NR_open];
  	sys_call_table[__NR_open] = our_sys_open;

	printk("Spying on UID:%d\n", uid);

	getuid_call = sys_call_table[__NR_getuid];

	return 0;
}

module_init(hello_init);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
