package main

import (
	"fmt"
	"reflect"
)

func getField() {
	s := struct {
		A int
		B int
	}{1, 2}
	// print the value 1
	field := reflect.ValueOf(s).Field(0)
	fmt.Println(field.Interface())
	// print the value 2
	field = reflect.ValueOf(s).Field(1)
	fmt.Println(field.Interface())

	// print the whole struct value
	fmt.Println(reflect.ValueOf(s))
}

func main() {

	var a int
	intType := reflect.TypeOf(a)
	fmt.Println(intType)

	// new a value from Type
	val := reflect.Zero(intType)
	fmt.Println(val)

	getField()
}
