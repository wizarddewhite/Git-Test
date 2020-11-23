package main

import (
	"fmt"
	"github.com/vishvananda/netlink"
)

func main() {
	links, err := netlink.LinkList()
	if err != nil {
		return
	}
	for _, link := range links {
		fmt.Println(link.Attrs().Name, ": ", link.Type())
	}
}
