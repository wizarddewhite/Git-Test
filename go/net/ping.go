package main

import (
	"fmt"
	"net"
	"os"
)

func main() {
	if len(os.Args) != 2 {
		fmt.Println("Usage: ", os.Args[0], "host")
		os.Exit(1)
	}

	addr, err := net.ResolveIPAddr("ip", os.Args[1])
	if err != nil {
		fmt.Println("Resolution error", err.Error())
		os.Exit(1)
	}

	laddr, err := net.ResolveIPAddr("ip", "0.0.0.0")
	fmt.Println("local addr: ", laddr, " remote addr: ", addr)
	conn, err := net.DialIP("ip4:icmp", laddr, addr)
	checkError(err)

	var msg [512]byte
	msg[0] = 8
	msg[1] = 0

	// CheckSum
	msg[2] = 0x86
	msg[3] = 0xf8

	msg[4] = 0x63
	msg[5] = 0x9c

	msg[6] = 0
	msg[7] = 1
	len := 8

	//check := checkSum(msg[0:len])
	//msg[2] = byte(check >> 8)
	//msg[3] = byte(check & 0xff)

	_, err = conn.Write(msg[0:len])
	checkError(err)

	_, err = conn.Read(msg[0:])
	checkError(err)

	fmt.Println("Got Response ")
	if msg[5] == 13 {
		fmt.Println("identifier matches")
	}
	if msg[7] == 37 {
		fmt.Println("Sequence matches")
	}

	os.Exit(0)
}

func checkSum(msg []byte) uint16 {
	sum := 0

	// assume even for now
	for n := 1; n < len(msg)-1; n += 2 {
		sum += int(msg[n])*256 + int(msg[n+1])
	}
	sum = (sum >> 16) + (sum & 0xffff)
	sum += (sum >> 16)
	var answer uint16 = uint16(^sum)
	return answer
}

func checkError(err error) {
	if err != nil {
		fmt.Println("Fatal error: ", err)
		os.Exit(1)
	}
}
