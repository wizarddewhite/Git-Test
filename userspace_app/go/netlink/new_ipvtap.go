package main

import (
	"fmt"
	"github.com/vishvananda/netlink"
	"os"
)

func main() {
	fd, err := os.Open("/var/run/netns/cni-9853c418-abae-27d7-66b3-c584af24302b")
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
}
