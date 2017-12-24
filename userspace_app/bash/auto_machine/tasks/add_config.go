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
	if len(os.Args) != 3 {
		return
	}

	name := os.Args[1]
	uuid := os.Args[2]

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
	c := make(map[string]interface{})
	c["id"] = uuid
	c["alterId"] = 64
	c["email"] = name
	clients = append(clients, c)
	settings["clients"] = clients
	config, _ := json.Marshal(&conf)
	err = ioutil.WriteFile("serv_config.json", config, 0644)
	if err != nil {
		fmt.Println(err)
	}
}
