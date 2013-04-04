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
#include <linux/percpu.h>
#include <net/dst.h>
MODULE_LICENSE("Dual BSD/GPL");

struct net_device *dev;

struct pcpu_lstats {
	u64			packets;
	u64			bytes;
	struct u64_stats_sync	syncp;
};

static u32 always_on(struct net_device *dev)
{
	return 1;
}

static const struct ethtool_ops yw_netdev_ethtool_ops = {
	.get_link		= always_on,
};

static int yw_netdev_init(struct net_device *dev)
{
	dev->lstats = alloc_percpu(struct pcpu_lstats);
	if (!dev->lstats)
		return -ENOMEM;

	return 0;
}

/*
 * The higher levels take care of making this non-reentrant (it's
 * called with bh's disabled).
 */
static netdev_tx_t yw_netdev_xmit(struct sk_buff *skb,
				 struct net_device *dev)
{
	struct pcpu_lstats *lb_stats;
	int len;

	skb_orphan(skb);

	/* Before queueing this packet to netif_rx(),
	 * make sure dst is refcounted.
	 */
	skb_dst_force(skb);

	skb->protocol = eth_type_trans(skb, dev);

	/* it's OK to use per_cpu_ptr() because BHs are off */
	lb_stats = this_cpu_ptr(dev->lstats);

	len = skb->len;
	if (likely(netif_rx(skb) == NET_RX_SUCCESS)) {
		u64_stats_update_begin(&lb_stats->syncp);
		lb_stats->bytes += len;
		lb_stats->packets++;
		u64_stats_update_end(&lb_stats->syncp);
	}

	return NETDEV_TX_OK;
}

static struct rtnl_link_stats64 *yw_netdev_get_stats64(struct net_device *dev,
						      struct rtnl_link_stats64 *stats)
{
	u64 bytes = 0;
	u64 packets = 0;
	int i;

	for_each_possible_cpu(i) {
		const struct pcpu_lstats *lb_stats;
		u64 tbytes, tpackets;
		unsigned int start;

		lb_stats = per_cpu_ptr(dev->lstats, i);
		do {
			start = u64_stats_fetch_begin_bh(&lb_stats->syncp);
			tbytes = lb_stats->bytes;
			tpackets = lb_stats->packets;
		} while (u64_stats_fetch_retry_bh(&lb_stats->syncp, start));
		bytes   += tbytes;
		packets += tpackets;
	}
	stats->rx_packets = packets;
	stats->tx_packets = packets;
	stats->rx_bytes   = bytes;
	stats->tx_bytes   = bytes;
	return stats;
}

static void yw_netdev_free(struct net_device *dev)
{
	free_percpu(dev->lstats);
	free_netdev(dev);
}

static const struct net_device_ops yw_netdev_ops = {
	.ndo_init      = yw_netdev_init,
	.ndo_start_xmit= yw_netdev_xmit,
	.ndo_get_stats64 = yw_netdev_get_stats64,
};

static void yw_netdev_setup(struct net_device *dev)
{
	ether_setup(dev);
	dev->mtu		= 64 * 1024;
	dev->hard_header_len	= ETH_HLEN;	/* 14	*/
	dev->addr_len		= ETH_ALEN;	/* 6	*/
	dev->tx_queue_len	= 0;
	dev->type		= ARPHRD_ETHER;
	dev->flags		= IFF_BROADCAST|IFF_MULTICAST;
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
	dev->ethtool_ops	= &yw_netdev_ethtool_ops;
	//dev->header_ops		= &eth_header_ops;
	dev->netdev_ops		= &yw_netdev_ops;
	dev->destructor		= yw_netdev_free;
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
