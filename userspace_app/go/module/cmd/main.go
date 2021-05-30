package main

import (
	"example.com/hello"
	"example.com/hello/animal"
	"fmt"
)

type Callback func(string)

var callbacks []Callback

func main() {
	callbacks = append(callbacks, animal.ChangeName)
	fmt.Println(hello.HelloHuman())
	fmt.Println(hello.HelloAnimal())

	for _, c := range callbacks {
		c("black")
	}

	fmt.Println(hello.HelloHuman())
	fmt.Println(hello.HelloAnimal())
}
