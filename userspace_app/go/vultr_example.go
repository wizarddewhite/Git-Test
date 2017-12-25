package main

import (
	vultr "github.com/JamesClonk/vultr/lib"
	"fmt"
	"os/exec"
	"time"
	"strconv"
)

type Node struct {
	Users    int
	IsMaster bool
	vultr.Server
}

var buffer int
var cand_nodes []Node
var busy_nodes []Node


func create() {
	client := vultr.NewClient("S4S4MTM6UQV4RMKWRM4DPM3NSROV3E5R35BA", nil)
	server, err := client.CreateServer("test", 7, 201, 241, nil)
	if err != nil {
		fmt.Println("Fatal error")
		fmt.Println(err)
		return
	}
	fmt.Println("Machine Created")
	fmt.Println(server.ID)
	time.Sleep(2 * time.Minute)
	for {
		server, err = client.GetServer(server.ID)
		if server.ServerState == "ok" {
			break
		}
		fmt.Println("1")
		time.Sleep(10 * time.Second)
	}
	fmt.Println("Machine Up:"+server.MainIP+":"+server.DefaultPassword)
	time.Sleep(10 * time.Second)
	err = client.DeleteServer(server.ID)
	fmt.Println(err)
	if err != nil && err.Error() == "Unable to destroy server: Servers cannot be destroyed within 5 minutes of being created" {
		fmt.Println("Try again")
	}
	return
}

func CreateNode() {
	var err error
	var server vultr.Server
	client := vultr.NewClient("S4S4MTM6UQV4RMKWRM4DPM3NSROV3E5R35BA", nil)
	server, err = client.CreateServer("test", 7, 201, 241, nil)
	if err != nil && err.Error() == "Plan is not available in the selected datacenter.  This could mean you have chosen the wrong plan (for example, a storage plan in a location that does not offer them), or the location you have selected does not have any more capacity." {
		server, err = client.CreateServer("test", 9, 201, 241, nil)
	}

	if err != nil {
		fmt.Println(err)
		return
	}
	fmt.Println(server.ID + " created")

	// wait for installation
	time.Sleep(3 * time.Minute)
		for {
			server, err = client.GetServer(server.ID)
			if server.ServerState == "ok" {
				break
			}
			fmt.Println("sleep")
			time.Sleep(10 * time.Second)
		}


	// dup_machine
	fmt.Println("start up setup")
	fmt.Println(server.MainIP)
	fmt.Println(server.DefaultPassword)

	port := strconv.FormatInt(32, 10)
	cmd := exec.Command("bash", "-c", "/root/dup_machine/dup_machine.sh "+server.MainIP+" '"+server.DefaultPassword+"' " + "abc " + port)
	output, err := cmd.Output()
	if err != nil {
		fmt.Println(err)
	}
	fmt.Printf("%s\n", output)
}

func getServers() {
	client := vultr.NewClient("S4S4MTM6UQV4RMKWRM4DPM3NSROV3E5R35BA", nil)
	servers, err := client.GetServers()
	if err != nil {
		fmt.Println("Fatal error")
		fmt.Println(err)
		return
	}

	for  _, serv := range servers {
		cand_nodes = append([]Node{Node{0, false, serv}}, cand_nodes...)
	}

	for _, node := range cand_nodes {
		fmt.Printf("%s: %s\n", node.Server.ID, node.IsMaster)
		fmt.Println(node.Server.AllowedBandwidth)
		fmt.Println(node.Server.CurrentBandwidth)
	}

	fmt.Println(len(cand_nodes))
	fmt.Println(len(busy_nodes))

	cand_nodes = append(cand_nodes[:0], cand_nodes[1:]...)
	fmt.Println(len(cand_nodes))
}

func main() {
	//create()
	CreateNode()
}

