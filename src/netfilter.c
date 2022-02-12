#include "../inc/netfilter.h"


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