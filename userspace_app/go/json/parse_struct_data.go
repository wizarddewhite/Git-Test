// Reference:
// https://www.sohamkamani.com/blog/2017/10/18/parsing-json-in-golang/
package main

import (
	"encoding/json"
	"fmt"
	"os"
	"time"
)

type Bird struct {
	Species     string
	Description string
}

func single_struct() {
	birdJson := `{"species": "pigeon","description": "likes to perch on rocks"}`
	var bird Bird
	json.Unmarshal([]byte(birdJson), &bird)
	fmt.Printf("Species: %s\nDescription: %s\n", bird.Species, bird.Description)
	//Species: pigeon, Description: likes to perch on rocks
}

func array() {
	birdJson := `[{"species":"pigeon","decription":"likes to perch on rocks"},{"species":"eagle","description":"bird of prey"}]`
	var birds []Bird
	json.Unmarshal([]byte(birdJson), &birds)
	fmt.Printf("Birds : %+v\n", birds)
	//Birds : [{Species:pigeon Description:} {Species:eagle Description:bird of prey}]
}

type Dimensions struct {
	Height int
	Width  int
}

type Bird2 struct {
	Species     string
	Description string
	Dimensions  Dimensions
}

func embedded_struct() {
	birdJson := `{"species":"pigeon","description":"likes to perch on rocks", "dimensions":{"height":24,"width":10}}`
	var bird Bird2
	json.Unmarshal([]byte(birdJson), &bird)
	fmt.Println(bird)
}

type Bird3 struct {
	Species     string `json:"birdType"`
	Description string `json:"what it does"`
}

func custom_attr_name() {
	// we could use a different attribute name (from the struct name) in json
	birdJson := `{"birdType": "pigeon","what it does": "likes to perch on rocks"}`
	var bird Bird3
	json.Unmarshal([]byte(birdJson), &bird)
	fmt.Println(bird)
}

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
	configs := []Config{}
	decoder := json.NewDecoder(file)
	decoder.Decode(&configs)
	for _, config := range configs {
		fmt.Println("ID:", config.Setting.ID)
		fmt.Println("Port:", config.Port)
		fmt.Println("Setting:", config.Settings.ID)
		fmt.Println("Process:", config.Process.User)
	}
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
	//single_struct()
	// array()
	// embedded_struct()
	// custom_attr_name()
	simple_test()
	// timestamp_parse()
}
