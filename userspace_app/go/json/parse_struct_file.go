// Reference:
// https://www.sohamkamani.com/blog/2017/10/18/parsing-json-in-golang/
package main

import (
	"encoding/json"
	"fmt"
	"os"
	"time"
)

type Setting struct {
	ID string `json:"id"`
}
type Process struct {
	User string `json:"user"`
}

type Config struct {
	Setting
	Port     uint16   `json:"port"`
	Settings Setting  `json:"settings"`
	Process  *Process `json:"process,omitempty"`
}

func simple_test() {
	file, _ := os.Open("json.conf")
	config := &Config{}
	decoder := json.NewDecoder(file)
	decoder.Decode(config)
	fmt.Println(config.Setting.ID)
	fmt.Println(config.Port)
	fmt.Println(config.Settings.ID)
	fmt.Println(config.Process.User)
}

type ObjStatus struct {
	Status string `json:"status,omitempty"`
	Reason string `json:"reason,omitempty"`

	Source string    `json:"source,omitempty"`
	Ts     time.Time `json:"timestamp,omitempty"`
}

func timestamp_parse() {
	file, _ := os.Open("json_time.status")
	status := &ObjStatus{}
	decoder := json.NewDecoder(file)
	decoder.Decode(status)
	fmt.Println(status.Status)
	fmt.Println(status.Ts.Format(time.RFC1123))
	fmt.Println(status.Ts.Format("2006-01-02 15:04:05"))
}

func main() {
	// simple_test()
	timestamp_parse()
}
