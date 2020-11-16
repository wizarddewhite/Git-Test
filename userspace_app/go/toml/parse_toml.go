package main

import (
	"fmt"
	"io/ioutil"

	"github.com/BurntSushi/toml"
)

type netmon struct {
	Path   string `toml:"path"`
	Debug  bool   `toml:"enable_debug"`
	Enable bool   `toml:"enable_netmon"`
}

type tomlConfig struct {
	Netmon netmon
}

func main() {
	var tomlConf tomlConfig

	configData, _ := ioutil.ReadFile("config.toml")
	// fmt.Println(string(configData)) // print whole file

	if _, err := toml.Decode(string(configData), &tomlConf); err != nil {
		fmt.Println(err)
		return
	}

	fmt.Printf("path %s\n", tomlConf.Netmon.Path)
}
