package main

import "fmt"

func add(x int, y int) int {
	return x + y
}

type World int

func (w *World) get_val() int {
	return int(*w) + 80
}

func main() {
	var w World
	w = 1
	fmt.Println(add(1, 3))
	fmt.Println("The World is ", w.get_val())
}
