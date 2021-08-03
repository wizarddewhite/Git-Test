package main

import (
	"bytes"
	"encoding/gob"
	"encoding/json"
	"fmt"
	"reflect"
)

type Vertex struct {
	Lat, Long float64
}

type VertexList map[string]Vertex

func newMap() {
	var vl VertexList
	fmt.Println("vl: ", vl)
	vl = make(VertexList)
	fmt.Println("vl: ", vl)
	vl["New"] = Vertex{
		12, 21,
	}
	fmt.Println("--- Iterate with name")
	for name := range vl {
		fmt.Println(name, vl[name])
	}
}

func general_operation() {
	var m map[string]Vertex
	if m == nil {
		fmt.Println("after definition map is nil, and following would crash")
		/*
			m["Bell Labs"] = Vertex{
				40.68433, -74.39967,
			}
		*/
	}
	m = make(map[string]Vertex)
	m["Bell Labs"] = Vertex{
		40.68433, -74.39967,
	}
	m["AT&T Labs"] = Vertex{
		40.68433, -74.39967,
	}
	fmt.Println(m["Bell Labs"])

	v, ok := m["AT&T Labs"]
	if ok {
		fmt.Println(v)
	} else {
		fmt.Println("No such element")
	}

	// iterate map
	fmt.Println("--- Iterate with name")
	for name := range m {
		fmt.Println(name, m[name])
	}

	// iterate map
	fmt.Println("--- Iterate with name and data")
	for name, data := range m {
		fmt.Println(name, data)
	}

	delete(m, "AT&T Labs")

	v, ok = m["AT&T Labs"]
	if ok {
		fmt.Println(v)
	} else {
		fmt.Println("No such element")
	}
}

func map_compare() {
	var m1 map[string]string
	var m2 map[string]string
	m1 = make(map[string]string)
	m2 = make(map[string]string)

	m1["key2"] = "value2"
	m1["key1"] = "value1"
	m2["key1"] = "value1"
	m2["key2"] = "value2"

	// iterate map first
	for k, v := range m1 {
		fmt.Print(k + " : ")
		fmt.Println(v)
	}
	for k, v := range m1 {
		fmt.Print(k + " : ")
		fmt.Println(v)
	}

	if reflect.DeepEqual(m1, m2) {
		fmt.Println(m1, "==", m2)
	} else {
		fmt.Println(m1, "!=", m2)
	}

	m1["key2"] = "other"
	if reflect.DeepEqual(m1, m2) {
		fmt.Println(m1, "==", m2)
	} else {
		fmt.Println(m1, "!=", m2)
	}

}

func map_serialize() {
	var m = map[string]int{"one": 1, "two": 2, "three": 3}
	fmt.Printf("origin map is: %#v\n", m)

	b := new(bytes.Buffer)

	e := gob.NewEncoder(b)

	// Encoding the map
	err := e.Encode(m)
	if err != nil {
		panic(err)
	}

	fmt.Println("map is encoded to: ", b.String())

	var decodedMap map[string]int
	d := gob.NewDecoder(b)

	// Decoding the serialized data
	err = d.Decode(&decodedMap)
	if err != nil {
		panic(err)
	}

	// Ta da! It is a map!
	fmt.Printf("%#v\n", decodedMap)
}

func map_json() {
	var m = map[string][]string{"one": {"1"}, "two": {"2"}, "three": {"3"}}
	fmt.Println(m)
	jsonString, _ := json.Marshal(m)
	fmt.Println(string(jsonString))

	n := make(map[string][]string)
	err := json.Unmarshal([]byte(jsonString), &n)
	if err != nil {
		panic(err)
	}

	fmt.Println(n)
}

type WithMap struct {
	Lat    int
	Proper map[string]string
}

func mapField() {
	v := WithMap{
		Lat: 3,
		Proper: map[string]string{
			"bad": "ddd",
		},
	}
	fmt.Println(v)
}

func main() {
	// general_operation()
	// newMap()
	// map_compare()
	// map_serialize()
	// map_json()
	mapField()
}
