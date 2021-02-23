package main

import "fmt"

type Vertex struct {
	X int
	Y int
}

func print_struct() {
	fmt.Println(Vertex{1, 2})
	fmt.Println(Vertex{X: 2, Y: 1})
}

type Meta struct {
	Name string
}

type Type struct {
	// Name string
	Kind string
}

type EmbedField struct {
	Type
	Meta
}

func embeded_field() {
	var ef EmbedField

	ef.Name = "embed"
	ef.Meta.Name = "embed"

	ef.Kind = "field"

	fmt.Println(ef)
}

func main() {
	// print_struct()
	embeded_field()
}
