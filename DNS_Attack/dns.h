#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <unistd.h>

typedef struct iphdr iph;
typedef struct udphdr udph; 


// Pseudoheader struct
typedef struct
{
    u_int32_t saddr;
    u_int32_t daddr;
    u_int8_t filler;
    u_int8_t protocol;
    u_int16_t len;
}ps_hdr;

// DNS header struct
typedef struct
{
	unsigned short id; 		// ID
	unsigned short flags;	// DNS Flags
	unsigned short qcount;	// Question Count
	unsigned short ans;		// Answer Count
	unsigned short auth;	// Authority RR
	unsigned short add;		// Additional RR
}dns_hdr;

// Question types
typedef struct
{
	unsigned short qtype;
	unsigned short qclass;
}query;

/*
typedef struct
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
*/
