package main

import "fmt"

func Hello() string {
	return "Hello, world"
}

func Add(a, b int) int {
	return a + b
}

func main() {
	fmt.Println(Hello())
}
