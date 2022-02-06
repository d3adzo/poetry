#include "syscall.h"

asmlinkage int our_sys_open(const char *filename, int flags, int mode)
{
	int i = 0;
	char ch;

	if (uid == getuid_call()) 
	{
		printk("Opened file by %d\n", uid);
		do 
		{
			get_user(ch, filename+i);
			i++;
			printk("%c", ch);

		} while (ch != 0);
		printk("\n");
		
		return original_call(filename, flags, mode);
	}
}
