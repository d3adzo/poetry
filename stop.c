#include "helper.h"

static void __exit hello_exit(void)
{
	printk(KERN_ALERT "Goodbye %s\n", evan);
}

module_exit(hello_exit);
