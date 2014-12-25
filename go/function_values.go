package main

import (
	"fmt"
	"math"
)

func my_func(x, y float64) float64 {
	return math.Sqrt(x*x + y*y)
}

func main() {
	hypot := my_func

	fmt.Println(hypot(3, 4))
	fmt.Println(my_func(3, 4))
}
