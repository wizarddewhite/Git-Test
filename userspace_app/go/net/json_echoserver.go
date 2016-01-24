package main

import (
	"encoding/json"
	"fmt"
	"net"
	"os"
)

type Name struct {
	Family   string
	Personal string
}

type Email struct {
	Kind    string
	Address string
}

type Person struct {
	Name  Name
	Email []Email
}

func (p Person) String() string {
	s := p.Name.Personal + " " + p.Name.Family
	for _, v := range p.Email {
		s += "\n" + v.Kind + ": " + v.Address
	}
	return s
}

func main() {
	service := "0.0.0.0:1200"
	tcpAddr, err := net.ResolveTCPAddr("tcp", service)
	checkError(err)

	listerner, err := net.ListenTCP("tcp", tcpAddr)
	checkError(err)

	for {
		conn, err := listerner.Accept()
		if err != nil {
			continue
		}

		encoder := json.NewEncoder(conn)
		decoder := json.NewDecoder(conn)

		for n := 0; n < 10; n++ {
			var person Person
			decoder.Decode(&person)
			fmt.Println(person)
			encoder.Encode(person)
		}
		conn.Close()
	}
}

func checkError(err error) {
	if err != nil {
		fmt.Println("Fatal error", err)
		os.Exit(1)
	}
}
