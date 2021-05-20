package main

import (
	"fmt"
	"reflect"
)

func main() {

	var a int
	intType := reflect.TypeOf(a)
	fmt.Println(intType)

	// new a value from Type
	val := reflect.Zero(intType)
	fmt.Println(val)
}
