package main

import (
	"fmt"
	"math"
)

type Vertex struct {
	X, Y float64
}

func (v *Vertex) Abs() float64 {
	return math.Sqrt(v.X*v.X + v.Y*v.Y)
}

func (v *Vertex) Abs_int(parm int) (float64, int) {
	return math.Sqrt(v.X*v.X + v.Y*v.Y), parm
}

//func (v Vertex) Scale(f float64)
func (v *Vertex) Scale(f float64) {
	v.X = v.X * f
	v.Y = v.Y * f
}

func main() {
	v := &Vertex{3, 4}
	fmt.Println(v.Abs())
	fmt.Println(v.Abs_int(2))

	v.Scale(5)
	fmt.Println(v.Abs())
}

