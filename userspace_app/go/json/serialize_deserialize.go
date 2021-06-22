package main

import (
	"encoding/json"
	"fmt"
)

type SubCate struct {
	Name     string `json:"name"`
	Location string `json:"location"`
}

type Bird struct {
	Species     string    `json:"species"`
	Description string    `json:"description"`
	SubCates    []SubCate `json:"subcate"`
}

func main() {
	bird := Bird{
		Species:     "Psittaciformes",
		Description: "diversity in South America and Australasia",
		SubCates: []SubCate{
			{
				Name:     "Platycercinae",
				Location: "Australia",
			},
		},
	}

	b, _ := json.Marshal(bird)
	fmt.Println("bird is serialized to: ", string(b))

	var b2 Bird
	json.Unmarshal(b, &b2)
	fmt.Println("string b is deserialized to: ", b2)

}
