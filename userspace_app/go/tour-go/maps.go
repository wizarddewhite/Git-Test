package main

import (
	"bytes"
	"encoding/gob"
	"encoding/json"
	"fmt"
	"reflect"
	"sync"
	"time"
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
		80.68433, -74.39967,
	}
	fmt.Println(m["Bell Labs"])

	if v, ok := m["AT&T Labs"]; !ok {
		fmt.Println("No such element")
	} else {
		fmt.Println(v)
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

	if v, ok := m["AT&T Labs"]; !ok {
		fmt.Println("No such element")
	} else {
		fmt.Println(v)
	}

	var im map[string]int
	im = make(map[string]int)
	im["height"] = 1

	if v, ok := im["height"]; !ok {
		fmt.Println("No such element: height")
	} else {
		fmt.Println("height is ", v)
	}

	if v, ok := im["height"]; !ok || v != 3 {
		fmt.Println("No such element: height")
	} else {
		fmt.Println("height is ", v)
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

func func_with_map(m map[string]string, key, val string) {
	m[key] = val
}

func map_parameter() {
	var m = map[string]string{"one": "1", "two": "2", "three": "3"}

	func_with_map(m, "one", "一")
	for key := range m {
		fmt.Println(key, m[key])
	}
}

func map_concurrent_crash() {
	c := make(map[string]int)
	go func() { //开一个协程写map
		for j := 0; j < 1000000; j++ {
			c[fmt.Sprintf("%d", j)] = j
		}
	}()
	go func() { //开一个协程读map
		for j := 0; j < 1000000; j++ {
			fmt.Println(c[fmt.Sprintf("%d", j)])
		}
	}()

	time.Sleep(time.Second * 20)
}

func map_concurrent_mutex() {
	var m sync.Mutex
	c := make(map[string]int)
	go func() { //开一个协程写map
		for j := 0; j < 1000000; j++ {
			m.Lock()
			c[fmt.Sprintf("%d", j)] = j
			m.Unlock()
		}
	}()
	go func() { //开一个协程读map
		for j := 0; j < 1000000; j++ {
			m.Lock()
			fmt.Println(c[fmt.Sprintf("%d", j)])
			m.Unlock()
		}
	}()

	time.Sleep(time.Second * 20)
}

type Lock struct {
	ch chan struct{}
}

func NewLock() *Lock {
	return &Lock{
		ch: make(chan struct{}, 1),
	}
}

func (t *Lock) Lock() {
	<-t.ch
}

func (t *Lock) Unlock() {
	t.ch <- struct{}{}
}

func map_concurrent_channel() {
	m := NewLock()
	m.ch <- struct{}{}
	c := make(map[string]int)
	go func() { //开一个协程写map
		for j := 0; j < 1000000; j++ {
			m.Lock()
			c[fmt.Sprintf("%d", j)] = j
			m.Unlock()
		}
	}()
	go func() { //开一个协程读map
		for j := 0; j < 1000000; j++ {
			m.Lock()
			fmt.Println(c[fmt.Sprintf("%d", j)])
			m.Unlock()
		}
	}()

	time.Sleep(time.Second * 20)
}

func main() {
	map_concurrent_channel()
	// map_concurrent_mutex()
	// map_concurrent_crash()
	// map_parameter()
	// general_operation()
	// newMap()
	// map_compare()
	// map_serialize()
	// map_json()
	// mapField()
}
