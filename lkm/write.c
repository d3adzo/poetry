extern int debug;

static asmlinkage long (*orig_write)(const struct pt_regs *);
asmlinkage long hook_write(const struct pt_regs *regs)
{

    char* kernel_filename;
	kernel_filename = kmalloc(4096, GFP_KERNEL);

	if(!kernel_filename)
	{
		return NULL;
	}

	if (strncpy_from_user(kernel_filename, (char*)regs->di, 4096) < 0) 
	{
		printk(KERN_WARNING "Error: strncpy\n");
		kfree(kernel_filename);
		return NULL;
	}
	
	printk(KERN_INFO "%s\n", kernel_filename);

    if (ststr(kernel_filename, "poet:") 
    {
        printk(KERN_INFO "No food for you!\n");
        kfree(kernel_filename);
        return EINTR;
    }

	kfree(kernel_filename);

    return orig_write(regs);

}
