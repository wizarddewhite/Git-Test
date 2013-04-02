/*
 * =====================================================================================
 *
 *       Filename:  register_netdev.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/02/2013 06:07:39 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
* =====================================================================================
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/etherdevice.h>
#include <linux/rtnetlink.h>
#include <net/rtnetlink.h>
#include <linux/if_arp.h>
MODULE_LICENSE("Dual BSD/GPL");

struct net_device *dev;

static const struct net_device_ops yw_netdev_ops = {
	.ndo_init      = NULL,
	.ndo_start_xmit= NULL,
	.ndo_get_stats64 = NULL,
};

static void yw_netdev_setup(struct net_device *dev)
{
	dev->mtu		= 64 * 1024;
	dev->hard_header_len	= ETH_HLEN;	/* 14	*/
	dev->addr_len		= ETH_ALEN;	/* 6	*/
	dev->tx_queue_len	= 0;
	dev->type		= ARPHRD_LOOPBACK;	/* 0x0001*/
	dev->flags		= IFF_LOOPBACK;
	dev->priv_flags	       &= ~IFF_XMIT_DST_RELEASE;
	dev->hw_features	= NETIF_F_ALL_TSO | NETIF_F_UFO;
	dev->features 		= NETIF_F_SG | NETIF_F_FRAGLIST
		| NETIF_F_ALL_TSO
		| NETIF_F_UFO
		| NETIF_F_HW_CSUM
		| NETIF_F_RXCSUM
		| NETIF_F_HIGHDMA
		| NETIF_F_LLTX
		| NETIF_F_NETNS_LOCAL
		| NETIF_F_VLAN_CHALLENGED
		| NETIF_F_LOOPBACK;
	//dev->ethtool_ops	= &yw_netdev_ethtool_ops;
	//dev->header_ops		= &eth_header_ops;
	dev->netdev_ops		= &yw_netdev_ops;
	//dev->destructor		= yw_netdev_free;
}

static int register_netdev_init(void)
{
	int err;

	pr_info("Net test: register a network device\n");

	err = -ENOMEM;
	dev = alloc_netdev(0, "yw_netdev", yw_netdev_setup);
	if (!dev)
		goto out;

	dev_net_set(dev, &init_net);

	err = register_netdev(dev);
	if (err)
		goto out_free_netdev;

	return 0;

out_free_netdev:
	free_netdev(dev);

out:
	return err;
}
static void register_netdev_exit(void)
{
	unregister_netdev(dev);
	free_netdev(dev);
}
module_init(register_netdev_init);
module_exit(register_netdev_exit);
