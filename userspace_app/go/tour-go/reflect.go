package main

import (
	"fmt"
	"reflect"
)

func getField() {
	s := struct {
		A int
		B int
	}{1, 0}
	// print the value 1
	field := reflect.ValueOf(s).Field(0)
	fmt.Println(field.Interface())
	// print the value 0
	field = reflect.ValueOf(s).Field(1)
	fmt.Println(field.Interface())

	// print the whole struct value
	var inf interface{}
	inf = s
	fmt.Println(reflect.ValueOf(inf))

	// below two have the same type
	fmt.Println(field.Type())
	fmt.Println(reflect.TypeOf(s.B))

	// use of Zero() method
	fmt.Println(reflect.Zero(field.Type()))
	fmt.Println(reflect.Zero(reflect.TypeOf(s.B)))

	// why return false?
	fmt.Println(reflect.DeepEqual(field.Interface(), reflect.Zero(field.Type())))
	fmt.Println(field.Interface() == reflect.Zero(field.Type()))
}

func getZero() {
	s := struct{ A int }{0}
	field := reflect.ValueOf(s).Field(0)

	// print 0
	fmt.Println("filed.Interface(): ", field.Interface())

	// use of Zero() method
	fmt.Println("reflect.Zero(): ", reflect.Zero(field.Type()))

	// use of Zero() method
	fmt.Println("TypeOf(field)            :", reflect.TypeOf(field))
	fmt.Println("TypeOf(field.Interface()):", reflect.TypeOf(field.Interface()))
	fmt.Println("TypeOf(reflect.Zero())   :", reflect.TypeOf(reflect.Zero(field.Type())))

	// use of Zero() method
	fmt.Println()
	fmt.Println("field.Interface() == reflect.Zero(field.Type()): ",
		reflect.DeepEqual(field.Interface(), reflect.Zero(field.Type())))

	fmt.Println("field.Interface() == int(reflect.Zero(field.Type().Int())): ",
		reflect.DeepEqual(field.Interface(), int(reflect.Zero(field.Type()).Int())))
	fmt.Println(reflect.DeepEqual(s, struct{ A int }{}))
	fmt.Println(reflect.DeepEqual(s, struct{ A int }{2}))
}

func tryValueOf() {
	a := []int{2, 5}

	var b reflect.Value = reflect.ValueOf(&a)
	b = b.Elem()
	fmt.Println("Slice :", a)

	//use of ValueOf method
	b = reflect.Append(b, reflect.ValueOf(80))
	fmt.Println("Slice after appending data:", b)
}

func newValueFromType() {
	// one way is to have a value of the type you want already
	a := 1
	// reflect.New works kind of like the built-in function new
	// We'll get a reflected pointer to a new int value
	fmt.Println("reflect.TypeOf(a):", reflect.TypeOf(a))
	intPtr := reflect.New(reflect.TypeOf(a))
	fmt.Printf("intPtr: %v, Type: %v\n", intPtr, reflect.TypeOf(intPtr))
	// Just to prove it
	b := intPtr.Elem().Interface().(int)
	// Prints 0
	fmt.Println(b)
}

func main() {
	// getField()
	// getZero()
	// tryValueOf()
	newValueFromType()
}
