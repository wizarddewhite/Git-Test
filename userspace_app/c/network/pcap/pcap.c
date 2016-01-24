/*
 * =====================================================================================
 *
 *       Filename:  pcap.c
 *
 *    Description:  reference from http://www.tcpdump.org/pcap.html
 *
 *        Version:  1.0
 *        Created:  04/28/2013 09:36:20 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <pcap.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void usage(char* argv[])
{
	printf("Usage: %s [-i dev_name]\n", argv[0]);
	printf("       -i: eth0\n");
	printf("Example:       \n");
	printf("        %s -i eth0  capture on eth0\n", argv[0]);
	return;
}

int main(int argc, char* argv[])
{
	int opt;
	char device_name[20] = {0};
	pcap_t *handle;
	char errbuf[PCAP_ERRBUF_SIZE];
	bpf_u_int32 mask;
	bpf_u_int32 net;
	char filter_exp[] = "port 23";
	struct bpf_program fp;
	struct pcap_pkthdr header;	/* The header that pcap gives us */
	const u_char *packet;		/* The actual packet */

	while ((opt = getopt(argc, argv, "i:")) != -1) {
		switch (opt) {
		case 'i':
			strncpy(device_name, optarg, sizeof(device_name)) ;
			break;
		default:
			fprintf(stderr, "Invalid option %c\n", opt);
			usage(argv);
			exit(EXIT_FAILURE);
		}
	}

	if (*device_name == '\0') {
		fprintf(stderr, "Missing device for capturing\n");
		usage(argv);
		exit(EXIT_FAILURE);
	}

	if (pcap_lookupnet(device_name, &net, &mask, errbuf) == -1) {
		fprintf(stderr, "Can't get netmask for device %s\n", device_name);
		net = 0;
		mask = 0;
	}

	handle = pcap_open_live(device_name, BUFSIZ, 1, 1000, errbuf);
	if (handle == NULL) {
		fprintf(stderr, "Couldn't open device %s: %s\n", device_name, errbuf);
		return(2);
	}

	if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
		fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
		return(2);
	}
	if (pcap_setfilter(handle, &fp) == -1) {
		fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
		return(2);
	}

	/* Grab a packet */
	packet = pcap_next(handle, &header);
	/* Print its length */
	printf("Jacked a packet with length of [%d]\n", header.len);
	/* And close the session */
	pcap_close(handle);

	return 0;
}
