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

func (t Type) type_show() {
	fmt.Println("type_show: ", t.Kind)
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

	// embedded field's method
	ef.type_show()
	ef.Type.type_show()
}

type EmbedFieldPointer struct {
	*Type
	Meta
}

func (efp *EmbedFieldPointer) embedded_method() {
	efp.type_show()
}

func embeded_field_pointer() {
	var efp EmbedFieldPointer
	var tp Type

	fmt.Println("")

	efp.Name = "embed"
	efp.Meta.Name = "embed"

	efp.Type = &tp
	efp.Kind = "efp Kind"

	fmt.Println(efp)

	// embedded field's method
	efp.type_show()
	efp.Type.type_show() // the same effect

	efp.Kind = "efp Kind from"
	efp.embedded_method()
}

func main() {
	// print_struct()
	// embeded_field()
	embeded_field_pointer()
}
