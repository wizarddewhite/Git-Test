package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"os"
)

var conf map[string]interface{}

type Client struct {
	ID    string `json:"id"`
	AID   int    `json:"alterId"`
	Email string `json:"email"`
}

func main() {
	if len(os.Args) != 2 {
		return
	}

	name := os.Args[1]

	data, err := ioutil.ReadFile("serv_config.json")
	if err != nil {
		fmt.Println(err)
		return
	}
	if err := json.Unmarshal(data, &conf); err != nil {
		fmt.Println(err)
		return
	}
	inbound := conf["inbound"].(map[string]interface{})
	settings := inbound["settings"].(map[string]interface{})
	clients := settings["clients"].([]interface{})

	for i, c := range clients {
		cm := c.(map[string]interface{})
		if cm["email"] == name {
			clients = append(clients[:i], clients[i+1:]...)
			break
		}
	}

	settings["clients"] = clients
	config, _ := json.Marshal(&conf)
	err = ioutil.WriteFile("serv_config.json", config, 0644)
	if err != nil {
		fmt.Println(err)
	}
}
