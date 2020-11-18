package main

import (
	"fmt"
	"math"
)

type Abser interface {
	Abs() float64
}

type Abber interface {
	Abb() float64
}

type MyFloat float64

func (f MyFloat) Abs() float64 {
	if f < 0 {
		return float64(-f)
	}
	return float64(f)
}

type Vertex struct {
	X, Y float64
}

func (v *Vertex) Abs() float64 {
	return math.Sqrt(v.X*v.X + v.Y*v.Y)
}

func (v *Vertex) Abb() float64 {
	return v.X + 10
}

func generic_type() {
	var a Abser
	var b Abber
	f := MyFloat(-math.Sqrt2)
	v := Vertex{3, 4}

	a = f  // a MyFloat implements Abser
	a = &v // a *Vertex implements Abser

	// In the following line, v is a Vertex (not *Vertex)
	// and does NOT implement Abser.
	//a = v

	fmt.Println(a.Abs())

	b = &v
	fmt.Println(b.Abb())
}

func main() {
	generic_type()
}
