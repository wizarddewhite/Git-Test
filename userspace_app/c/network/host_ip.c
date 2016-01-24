/*
 * =====================================================================================
 *
 *       Filename:  host_ip.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  02/19/2014 05:44:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

int main (int argc, const char * argv[]) {
	struct ifaddrs * ifAddrStruct=NULL;
	void * tmpAddrPtr=NULL;

	/* The  getifaddrs() function creates a linked list of structures
	 * describing the network interfaces of the local system
	 */
	getifaddrs(&ifAddrStruct);

/*
struct ifaddrs {
    struct ifaddrs  *ifa_next;    // Next item in list
    char            *ifa_name;    // Name of interface
    unsigned int     ifa_flags;   // Flags from SIOCGIFFLAGS
    struct sockaddr *ifa_addr;    // Address of interface
    struct sockaddr *ifa_netmask; // Netmask of interface
    union {
        struct sockaddr *ifu_broadaddr;
                         // Broadcast address of interface
        struct sockaddr *ifu_dstaddr;
                         // Point-to-point destination address
    } ifa_ifu;
#define              ifa_broadaddr ifa_ifu.ifu_broadaddr
#define              ifa_dstaddr   ifa_ifu.ifu_dstaddr
    void            *ifa_data;    // Address-specific data
};
*/

	while (ifAddrStruct!=NULL) {

	if (ifAddrStruct->ifa_addr->sa_family==AF_INET) { // check it is IP4
		// is a valid IP4 Address
		tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
		char addressBuffer[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
		printf("%s IP Address %s\n", ifAddrStruct->ifa_name, addressBuffer);
	} else if (ifAddrStruct->ifa_addr->sa_family==AF_INET6) { // check it is IP6
		// is a valid IP6 Address
		tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
		char addressBuffer[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
		printf("%s IP Address %s\n", ifAddrStruct->ifa_name, addressBuffer);
	}

	ifAddrStruct=ifAddrStruct->ifa_next;
	}
	return 0;
}
