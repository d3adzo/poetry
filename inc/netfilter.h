#include "ftrace_hook.h"

#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

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