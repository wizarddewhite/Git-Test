package main

import (
	"encoding/json"
	"fmt"
	"os"
)

type Setting struct {
	ID string `json:"id"`
}

type Config struct {
	Port     uint16  `json:"port"`
	Settings Setting `json:"settings"`
}

func main() {
	file, _ := os.Open("json.conf")
	config := &Config{}
	decoder := json.NewDecoder(file)
	decoder.Decode(config)
	fmt.Println(config.Port)
	fmt.Println(config.Settings.ID)
}
