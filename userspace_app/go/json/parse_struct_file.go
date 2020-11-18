// Reference:
// https://www.sohamkamani.com/blog/2017/10/18/parsing-json-in-golang/
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
	Setting
	Port     uint16  `json:"port"`
	Settings Setting `json:"settings"`
}

func simple_test() {
	file, _ := os.Open("json.conf")
	config := &Config{}
	decoder := json.NewDecoder(file)
	decoder.Decode(config)
	fmt.Println(config.Setting.ID)
	fmt.Println(config.Port)
	fmt.Println(config.Settings.ID)
}

func main() {
	simple_test()
}
