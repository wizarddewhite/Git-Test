/*
 * =====================================================================================
 *
 *       Filename:  rename_interface.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/02/2014 12:09:26 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

struct interface {
    int     index;
    int     flags;      /* IFF_UP etc. */
    long    speed;      /* Mbps; -1 is unknown */
    int     duplex;     /* DUPLEX_FULL, DUPLEX_HALF, or unknown */
    char    name[IF_NAMESIZE + 1];
};

static int get_interface_common(const int fd, struct ifreq *const ifr, struct interface *const info)
{
    struct ethtool_cmd  cmd;

    /* Interface flags. */
    if (ioctl(fd, SIOCGIFFLAGS, ifr) == -1)
        info->flags = 0;
    else
        info->flags = ifr->ifr_flags;

    ifr->ifr_data = (void *)&cmd;
    cmd.cmd = ETHTOOL_GSET; /* "Get settings" */
    if (ioctl(fd, SIOCETHTOOL, ifr) == -1) {
        /* Unknown */
        info->speed = -1L;
        info->duplex = DUPLEX_UNKNOWN;
    } else {
        info->speed = ethtool_cmd_speed(&cmd);
        info->duplex = cmd.duplex;
    }

    return 0;
}

int rename_interface(const char *const name, const char *const nname)
{
    int             socketfd, result;
    struct ifreq    ifr;
    struct interface    info;

    if (!name || !*name)
        return errno = EINVAL;

    socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (socketfd == -1)
        return errno;

    strncpy(ifr.ifr_name, name, IF_NAMESIZE);
    if (ioctl(socketfd, SIOCGIFINDEX, &ifr) == -1) {
        do {
		printf("closed\n");
            result = close(socketfd);
        } while (result == -1 && errno == EINTR);
        return errno = ENOENT;
    }

    info.index = ifr.ifr_ifindex;
    strncpy(info.name, name, IF_NAMESIZE);
    info.name[IF_NAMESIZE] = '\0';

    get_interface_common(socketfd, &ifr, &info);

    printf("%s: Interface %d", info.name, info.index);
    if (info.flags & IFF_UP)
        printf(", up");
    if (info.duplex == DUPLEX_FULL)
        printf(", full duplex");
    else
    if (info.duplex == DUPLEX_HALF)
        printf(", half duplex");
    if (info.speed > 0)
        printf(", %ld Mbps", info.speed);
    printf("\n");

    strncpy(ifr.ifr_newname, nname, IF_NAMESIZE);
    if ((result = ioctl(socketfd, SIOCSIFNAME, &ifr))) {
	    perror("Can't change interface: name");
	    printf("result %d\n", result);
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int                 status = 0;

    if (argc != 3) {
        fprintf(stderr, "\n");
        fprintf(stderr, "Usage: %s old_dev new_dev\n", argv[0]);
        fprintf(stderr, "\n");
        return 1;
    }

    if (rename_interface(argv[1], argv[2]) != 0) {
        fprintf(stderr, "%s: No such interface.\n", argv[1]);
        return -1;
    }

    return status;
}
