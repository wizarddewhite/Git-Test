// Reference:
// https://www.sohamkamani.com/blog/2017/10/18/parsing-json-in-golang/
package main

import (
	"encoding/json"
	"fmt"
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

func main() {
	//single_struct()
	// array()
	// embedded_struct()
	custom_attr_name()
}
