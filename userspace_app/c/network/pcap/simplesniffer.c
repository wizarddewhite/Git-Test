/* Simple Raw Sniffer                                                    */ 
/* Author: Luis Martin Garcia. luis.martingarcia [.at.] gmail [d0t] com  */
/* To compile: gcc simplesniffer.c -o simplesniffer -lpcap               */ 
/* Run as root!                                                          */ 
/*                                                                       */
/* This code is distributed under the GPL License. For more info check:  */
/* http://www.gnu.org/copyleft/gpl.html                                  */

/* This code is copied from
 * http://www.programming-pcap.aldabaknocking.com/code/simplesniffer.c
 */

#include <pcap.h> 
#include <string.h> 
#include <stdlib.h> 
#include <ctype.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXBYTES2CAPTURE 2048 

/* processPacket(): Callback function called by pcap_loop() everytime a packet */
/* arrives to the network card. This function prints the captured raw data in  */
/* hexadecimal.                                                                */
void processPacket(u_char *arg, const struct pcap_pkthdr* pkthdr, const u_char * packet)
{ 
 int i=0, *counter = (int *)arg; 
 struct ether_header *ethernet;
 struct ip *ip;

 ethernet = (struct ether_header *)packet;
 ip = (struct ip*)(packet + ETHER_HDR_LEN);


 printf("Packet Count: %d\n", ++(*counter)); 
 printf("Received Packet Size: %d\n", pkthdr->len); 
 printf("Payload:\n"); 
 printf("SRC: %s\n", inet_ntoa(ip->ip_src));
 printf("DST: %s\n", inet_ntoa(ip->ip_dst));
 for (i=0; i<pkthdr->len; i++){ 

    if ( isprint(packet[i]) ) /* If it is a printable character, print it */
        printf("%c ", packet[i]); 
    else 
        printf(". "); 
    
     if( (i%16 == 0 && i!=0) || i==pkthdr->len-1 ) 
        printf("\n"); 
  } 
 return; 
} 



/* main(): Main function. Opens network interface and calls pcap_loop() */
int main(int argc, char *argv[] )
{ 
 int i=0, count=0; 
 pcap_t *descr = NULL; 
 char errbuf[PCAP_ERRBUF_SIZE], *device=NULL; 
 memset(errbuf,0,PCAP_ERRBUF_SIZE); 

 if( argc > 1){  /* If user supplied interface name, use it. */
    device = argv[1];
 }
 else{  /* Get the name of the first device suitable for capture */ 

    if ( (device = pcap_lookupdev(errbuf)) == NULL){
        fprintf(stderr, "ERROR: %s\n", errbuf);
        exit(1);
    }
 }

 printf("Opening device %s\n", device); 
 
 /* Open device in promiscuous mode */ 
 if ( (descr = pcap_open_live(device, MAXBYTES2CAPTURE, 1,  512, errbuf)) == NULL){
    fprintf(stderr, "ERROR: %s\n", errbuf);
    exit(1);
 }

 /* Loop forever & call processPacket() for every received packet*/ 
 if ( pcap_loop(descr, -1, processPacket, (u_char *)&count) == -1){
    fprintf(stderr, "ERROR: %s\n", pcap_geterr(descr) );
    exit(1);
 }

return 0; 

} 

/* EOF*/
