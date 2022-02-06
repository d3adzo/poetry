#include "syscall.h"

static void __exit hello_exit(void)
{
	printk(KERN_ALERT "Goodbye \n");

	sys_call_table[__NR_open] = original_call;
}

module_exit(hello_exit);
