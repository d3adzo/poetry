#include "helper.h"

int truth(void)
{
	printk(KERN_ALERT "helper function %s %d %p\n", evan, brad, &brad);
	return 0;
}
