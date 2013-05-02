/*
 * =====================================================================================
 * http://backreference.org/2010/03/26/tuntap-interface-tutorial/
 *
 * 1. tap interface outputs (and must be given) full ethernet frames
 * 2. tun interface outputs (and must be given) raw IP packets 
 *    (and no ethernet headers are added by the kernel).
 * =====================================================================================
 */

#include <net/if.h>
#include <linux/if_tun.h> 
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <sys/time.h>

int tun_create(char* dev, int flags) 
{ 
    struct ifreq ifr; 
    int fd, err; 

    assert(dev != NULL); 

    if ((fd = open( "/dev/net/tun" , O_RDWR)) < 0) 
        return fd; 

    memset (&ifr, 0, sizeof(ifr)) ; 
    ifr.ifr_flags |= flags; 
    if (*dev != '\0') 
        strncpy (ifr.ifr_name, dev, IFNAMSIZ) ; 
    if ( (err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) { 
        close(fd) ; 
        return err; 
    } 
    strcpy (dev, ifr.ifr_name) ; 

    return fd; 
} 

void usage(char* argv[])
{
	printf("Usage: %s [-n name] [-t type] [-p] [-r file]\n", argv[0]);
	printf("       -t: tap or tun\n");
	printf("       -p: persist\n");
	printf("       -r: record packet to file\n");
	printf("Example:       \n");
	printf("        %s -n tun1 -t tap -p  create tap device tun1\n", argv[0]);
	printf("        %s -n tun1 -t tap     remove tap device tun1\n", argv[0]);
	printf("        %s -n tun1 -r file    capture packet to file\n", argv[0]);
	return;
}

int main(int argc, char * argv[]) 
{ 
        int tun, ret; 
        char tun_name[IFNAMSIZ]  = {0};
        unsigned char buf[4096] ; 
	int opt;
	int flags = IFF_TAP;
	int persist = 0;
	char *record_file = NULL;
	int fd_record = -1;
	struct pcap_file_header hdr;
	struct pcap_pkthdr      pkthdr;
	struct timeval t;

	while ((opt = getopt(argc, argv, "n:t:r:p")) != -1) {
		switch (opt) {
		case 'n':
			strncpy(tun_name, optarg, IFNAMSIZ) ;
			break;
		case 't':
			if (!strcmp("tap", optarg))
				flags = IFF_TAP;
			else if (!strcmp("tun", optarg))
				flags = IFF_TUN;
			else {
				fprintf(stderr, "Invalid option -%c %s\n",
						opt, optarg);
				usage(argv);
				exit(EXIT_FAILURE);
			}

			break;
		case 'p':
			persist = 1;
			break;
		case 'r':
			record_file = optarg;
			break;
		default:
			fprintf(stderr, "Invalid option %c\n", opt);
			usage(argv);
			exit(EXIT_FAILURE);
		}
	}

        tun = tun_create(tun_name, flags | IFF_NO_PI) ;
        if ( tun < 0) { 
                perror ( "tun_create" ) ; 
                return 1; 
        } 
        printf ( "TUN name is %s\n" , tun_name) ; 

	if (persist)
		ioctl(tun, TUNSETPERSIST, 1);
	else
		ioctl(tun, TUNSETPERSIST, 0);

	if (record_file) {
		printf("record to file: %s\n", record_file);
		fd_record = open(record_file, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);

		hdr.magic = 0xa1b2c3d4;
		hdr.version_major = 2;
		hdr.version_minor = 4;
		hdr.thiszone = 0;
		hdr.sigfigs = 0;
		hdr.snaplen = 65536;
		hdr.linktype = 1;

		write(fd_record, &hdr, sizeof(hdr));
	}

        while (1) { 
                unsigned char ip[ 4] ; 

                ret = read (tun, buf, sizeof (buf) ) ; 
                if ( ret < 0) 
                        break ; 

		if (fd_record >= 0) {
			gettimeofday(&t, NULL);

			pkthdr.ts.tv_sec = t.tv_sec;
			pkthdr.ts.tv_usec = t.tv_usec;
			pkthdr.caplen = ret;
			pkthdr.len = ret;

			write(fd_record, &pkthdr, sizeof(pkthdr));

			write(fd_record, buf, ret);
		}

                memcpy (ip, & buf[12] , 4) ; 
                memcpy (&buf[12], & buf[16] , 4) ; 
                memcpy (&buf[16], ip, 4) ; 
                buf[20] = 0; 
                *((unsigned short*)&buf[22] ) += 8; 
                printf ( "read %d bytes\n" , ret) ; 
                ret = write(tun, buf, ret) ; 
                printf ( "write %d bytes\n" , ret) ; 
        } 

        return 0; 
} 
