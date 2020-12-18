package main

import (
	"fmt"
	"github.com/containernetworking/plugins/pkg/ns"
	"github.com/vishvananda/netlink"
	"golang.org/x/sys/unix"
	"net"
	"os"
)

func main() {
	fd, err := os.Open("/var/run/netns/ns0")
	if err != nil {
		fmt.Println("Failed to open netns fd ", err)
		return
	}

	mv := &netlink.IPVtap{netlink.IPVlan{
		LinkAttrs: netlink.LinkAttrs{
			MTU:         1500,
			Name:        "foo",
			ParentIndex: 2,
		},
		Mode: netlink.IPVLAN_MODE_L2,
	}}

	err = netlink.LinkAdd(mv)
	if err != nil {
		fmt.Printf("could not add %s: %v\n", "foo", err)
	}

	netlink.LinkSetNsFd(mv, int(fd.Fd()))

	netns, err := ns.GetNS("/var/run/netns/ns0")
	if err != nil {
		fmt.Println("Failed to get netns", err)
		return
	}
	defer netns.Close()

	err = netns.Do(func(_ ns.NetNS) error {
		// link up lo
		lo, _ := netlink.LinkByName("lo")
		err = netlink.LinkSetUp(lo)
		if err != nil {
			return err
		}

		// assign IP address
		addr, _ := netlink.ParseAddr("169.254.1.2/32")
		err = netlink.LinkSetUp(mv)
		if err != nil {
			return err
		}
		err := netlink.AddrAdd(mv, addr)
		if err != nil {
			return err
		}

		// remove the route entry in local table
		err = netlink.RouteDel(&netlink.Route{
			// LinkIndex: mv.Attrs().Index,
			Src: net.IPv4(169, 254, 1, 2),
			Dst: &net.IPNet{
				IP:   net.IPv4(169, 254, 1, 2),
				Mask: net.CIDRMask(32, 32),
			},
			Scope: unix.RT_SCOPE_HOST,
			Table: unix.RT_TABLE_LOCAL,
		})
		fmt.Println("Failed local route", err)
		return err
	})

	if err != nil {
		fmt.Println("Failed to setup ip and route %v", err)
	}
}
