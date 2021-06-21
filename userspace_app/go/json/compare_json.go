package main

import (
	"encoding/json"
	"fmt"
	"io"
	"reflect"
)

// JSONEqual compares the JSON from two Readers.
func JSONEqual(a, b io.Reader) (bool, error) {
	var j, j2 interface{}
	d := json.NewDecoder(a)
	if err := d.Decode(&j); err != nil {
		return false, err
	}
	d = json.NewDecoder(b)
	if err := d.Decode(&j2); err != nil {
		return false, err
	}
	return reflect.DeepEqual(j2, j), nil
}

// JSONBytesEqual compares the JSON in two byte slices.
func JSONBytesEqual(a, b []byte) (bool, error) {
	var j, j2 interface{}
	if err := json.Unmarshal(a, &j); err != nil {
		return false, err
	}
	if err := json.Unmarshal(b, &j2); err != nil {
		return false, err
	}
	return reflect.DeepEqual(j2, j), nil
}

func main() {
	a := []byte(`{"x": ["y",42]}`)
	b := []byte(`{"x":                  ["y",  42]}`)
	c := []byte(`{"z": ["y", "42"]}`)
	empty := []byte{}
	bad := []byte(`{this? this is a test.}`)

	eq, err := JSONBytesEqual(a, b)
	fmt.Println("a=b\t", eq, "with error", err)
	eq, err = JSONBytesEqual(a, c)
	fmt.Println("a=c\t", eq, "with error", err)
	eq, err = JSONBytesEqual(a, empty)
	fmt.Println("a=empty\t", eq, "with error", err)
	eq, err = JSONBytesEqual(a, bad)
	fmt.Println("a=bad\t", eq, "with error", err)
}
