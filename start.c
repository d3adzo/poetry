#include "helper.h"

#define DRIVER_AUTHOR "Brad Hacker"
#define DRIVER_DESC "drivermode"

static int __init hello_init(void)
{
	int variable;

	printk(KERN_ALERT "evan is %s %p \n", evan, &evan);
	pr_info("hello from pr info\n");
	variable = truth();
	printk(KERN_ALERT "return val %d\n", variable);
	printk(KERN_ALERT "testing\n");
	return 0;
}

module_init(hello_init);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
