package main

import (
	"fmt"
)

type Vertex struct {
	Lat, Long float64
}

func main() {
	var m map[string]Vertex
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

	delete(m, "AT&T Labs")

	v, ok = m["AT&T Labs"]
	if ok {
		fmt.Println(v)
	} else {
		fmt.Println("No such element")
	}

}
