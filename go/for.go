package main

import "fmt"

func main() {
	var sum int
	sum = 0
	for i := 0; i < 10; i++ {
		sum += i
	}
	fmt.Println(sum)

	sum = 1
	for sum < 1000 {
		sum += sum
	}
	fmt.Println(sum)

	var a [10]int
	for i, val := range a {
		fmt.Printf("Value[%d] %d\n", i, val)
	}

	for _, val := range a {
		fmt.Println("Value ", val)
	}
}
