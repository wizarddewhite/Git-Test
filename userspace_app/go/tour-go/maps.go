package main

import (
	"fmt"
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
func main() {
	// general_operation()
	newMap()
}
