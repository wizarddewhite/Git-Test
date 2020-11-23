package main

import (
	"fmt"
	"github.com/vishvananda/netlink"
)

func main() {
	mv := &netlink.IPVlan{
		LinkAttrs: netlink.LinkAttrs{
			MTU:         1500,
			Name:        "foo",
			ParentIndex: 2,
		},
		Mode: netlink.IPVLAN_MODE_L2,
	}
	err := netlink.LinkAdd(mv)
	if err != nil {
		fmt.Printf("could not add %s: %v\n", "foo", err)
	}
}
