package main

import "fmt"

func for_loop() {
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

func nested_loop() {
	fmt.Println("nested break loop")
	for i := 0; i < 10; i++ {
	out1:
		for j := 0; j < 11; j++ {
			if i+j == 2 {
				fmt.Printf("%d %d\n", i, j)
				break out1
			}
		}
		fmt.Println("count i:", i)
	}

	fmt.Println("nested continue loop")
out2:
	for i := 0; i < 10; i++ {
		for j := 0; j < 11; j++ {
			if i+j == 2 {
				fmt.Printf("%d %d\n", i, j)
				continue out2
			}
		}
		fmt.Println("count i:", i)
	}
}

func main() {
	// for_loop()
	nested_loop()
}
