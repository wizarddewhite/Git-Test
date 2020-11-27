package main

import (
	"fmt"
	"golang.org/x/sys/unix"
	"net"

	. "github.com/vishvananda/netlink"
)

func test() {
	// get loopback interface
	link, err := LinkByName("ipvt0")
	if err != nil {
		fmt.Println("no ipvt0")
		return
	}

	// bring the interface up
	if err := LinkSetUp(link); err != nil {
		fmt.Println("ipvt0 not up")
		return
	}

	// add a gateway route
	dst := &net.IPNet{
		IP: net.IPv4(0, 0, 0, 0),
		// Mask: net.CIDRMask(16, 32),
	}

	// ip := net.IPv4(10, 10, 1, 2)
	route := Route{
		LinkIndex: link.Attrs().Index,
		Dst:       dst,
		Scope:     unix.RT_SCOPE_LINK,
	}
	if err := RouteAdd(&route); err != nil {
		fmt.Println("route add failed, ", err)
		return
	}

	routes, err := RouteList(link, FAMILY_V4)
	if err != nil {
		fmt.Println("No route ", err)
		return
	}
	fmt.Println("Current route table: ")
	for _, r := range routes {
		fmt.Println(r)
	}
}

func main() {
	test()
}
