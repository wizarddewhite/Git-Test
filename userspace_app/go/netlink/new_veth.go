package main

import (
	"fmt"
	"github.com/vishvananda/netlink"
)

func main() {
	veth := &netlink.Veth{
		LinkAttrs: netlink.LinkAttrs{
			MTU:         1500,
			Name:        "veth1",
			ParentIndex: 2,
			TxQLen:      50,
		},
		PeerName: "veth0",
	}
	err := netlink.LinkAdd(veth)
	if err != nil {
		fmt.Printf("could not add %s: %v\n", "foo", err)
	}
}
