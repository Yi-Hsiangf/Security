#include "dns.h"
//EDNS
// 讓struct可以不會剩下多餘的空間，方便讀取位址
#pragma pack(1)
#pragma pack(push)
struct dnsopt
{
	//dns_opt 
	//EDNS
	unsigned char name;
	unsigned short dnsclass;
	unsigned short udplen;
	
	//32位擴展返回狀態碼
	unsigned char rcode;  //8位擴展返回狀態碼
	unsigned char EDNS_ver; // EDNS版本
	unsigned short Z; //16bit Z
	
	//16位可變部分長度
	unsigned short datalen;
}dnsopt;
#pragma pack()	

unsigned short csum(unsigned short *ptr,int nbytes) 
{
	register long sum;
	unsigned short oddbyte;
	register short answer;

	sum=0;
	while(nbytes>1) {
		sum+=*ptr++;
		nbytes-=2;
	}
	if(nbytes==1) {
		oddbyte=0;
		*((unsigned char *)&oddbyte)=*(unsigned char *)ptr;
		sum+=oddbyte;
	}

	sum = (sum>>16)+(sum & 0xffff);
	sum = sum + (sum>>16);
	answer=(short)~sum;
	
	return(answer);
}

void DNS_format(unsigned char * dns,unsigned char * host) 
{
	int lock = 0 , i;
	strcat((char*)host,".");
	for(i = 0 ; i < strlen((char*)host) ; i++) 
	{
		if(host[i]=='.') 
		{
			*dns++ = i-lock;
			for(;lock<i;lock++) 
			{
				*dns++=host[lock];
			}
			lock++;
		}
	}
	*dns++=0x00;
}

// Creates the dns header and packet
void Create_DNS_header(dns_hdr *dns)
{
	//dns->id = (unsigned short) htons(getpid());
	dns->id = rand();
	dns->flags = htons(0x0100);
	dns->qcount = htons(1);
	dns->ans = htons(0);
	dns->auth = htons(0);
	dns->add = htons(1); //for addition
}

void Creat_DNS_OPT(struct dnsopt *dns_opt)
{
	dns_opt->name = 0;
	dns_opt->dnsclass = htons(41);
	dns_opt->udplen = htons(4096);
	dns_opt->rcode = 0;
	dns_opt->EDNS_ver = 0;
	dns_opt->Z = htons(0x8000);
	dns_opt->datalen = 0;
}

void dns_send(char *trgt_ip, int trgt_p, char *dns_srv, int dns_p, unsigned char *dns_record)
{
	// Building the DNS request data packet
	
	
	//header
	unsigned char dns_data[200];
	
	dns_hdr *dns = (dns_hdr *)&dns_data;
	Create_DNS_header(dns);
	
	
	unsigned char *dns_name, dns_rcrd[32];
	dns_name = (unsigned char *)&dns_data[sizeof(dns_hdr)];
	strcpy(dns_rcrd, dns_record);
	DNS_format(dns_name , dns_rcrd);

	//question type
	query *q;
	q = (query *)&dns_data[sizeof(dns_hdr) + (strlen(dns_name)+1)];
	q->qtype = htons(0x00ff);
	q->qclass = htons(0x1);

	
	// additional record section
	struct dnsopt *dns_opt;
	dns_opt = (struct dnsopt *)(&dns_data[sizeof(dns_hdr) + (strlen(dns_name)+1)] + sizeof(query));
	Creat_DNS_OPT(dns_opt);

	
	
	char datagram[4096], *data;
    memset(datagram, 0, 4096);
    
	data = datagram + sizeof(iph) + sizeof(udph);
    memcpy(data, &dns_data, sizeof(dns_hdr) + (strlen(dns_name)+1) + sizeof(query) + sizeof(dnsopt));
    
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;					//address family
    sin.sin_port = htons(dns_p);				//dns port
    sin.sin_addr.s_addr = inet_addr(dns_srv);     
	
	//build IP header
    iph *ip = (iph *)datagram;
    ip->version = 4;
    ip->ihl = 5;
    ip->tos = 0;
    ip->tot_len = sizeof(iph) + sizeof(udph) + sizeof(dns_hdr) + (strlen(dns_name)+1) + sizeof(query) + sizeof(dnsopt);
    ip->id = htonl(getpid());
    ip->frag_off = 0;
    ip->ttl = 64;
    ip->protocol = IPPROTO_UDP;
    ip->check = 0;
    ip->saddr = inet_addr(trgt_ip);
    ip->daddr = sin.sin_addr.s_addr;
	ip->check = csum((unsigned short *)datagram, ip->tot_len);
	
	//build UDP header
    udph *udp = (udph *)(datagram + sizeof(iph));
	udp->source = htons(trgt_p);
    udp->dest = htons(dns_p);
    udp->len = htons(8+sizeof(dns_hdr)+(strlen(dns_name)+1) + sizeof(query) + sizeof(dnsopt));
    udp->check = 0;
	
	// Build Pseudoheader
	// Checksum calculation
	ps_hdr pshdr;
	pshdr.saddr = inet_addr(trgt_ip);
    pshdr.daddr = sin.sin_addr.s_addr;
    pshdr.filler = 0;
    pshdr.protocol = IPPROTO_UDP;
    pshdr.len = htons(sizeof(udph) + sizeof(dns_hdr) + (strlen(dns_name)+1) + sizeof(query) + sizeof(dnsopt));

	int pssize = sizeof(ps_hdr) + sizeof(udph) + sizeof(dns_hdr) + (strlen(dns_name)+1) + sizeof(query) + sizeof(dnsopt);
    char *psgram = malloc(pssize);
	
    memcpy(psgram, (char *)&pshdr, sizeof(ps_hdr));
    memcpy(psgram + sizeof(ps_hdr), udp, sizeof(udph) + sizeof(dns_hdr) + (strlen(dns_name)+1) + sizeof(query) + sizeof(dnsopt));
		
    udp->check = csum((unsigned short *)psgram, pssize);
    
    // Send data
    int sd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if(sd < 0) 
		printf("sockter error");
    else 
		sendto(sd, datagram, ip->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin));
    
	free(psgram);
	close(sd);
	
	return;
}
