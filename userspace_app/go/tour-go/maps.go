package main

import (
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

func main() {
	// general_operation()
	// newMap()
	map_compare()
}
