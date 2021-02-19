package main

import "fmt"

func sum(a []int, c chan int) {
	sum := 0
	for _, v := range a {
		sum += v
	}
	c <- sum
}

func non() {
	a := make(chan int, 1)
	a <- 4
	fmt.Println(<-a)
}

func test_channel() {
	a := []int{7, 2, 8, -9, 4, 0}

	c := make(chan int)
	go sum(a[:len(a)/2], c)
	go sum(a[len(a)/2:], c)
	x, y := <-c, <-c

	fmt.Println(x, y, x+y)
	non()
}

func consumer1(in chan int) {
	for val := range in {
		fmt.Println("consumer1 see ", val)
	}
}

func consumer2(in chan int) {
	for val := range in {
		fmt.Println("consumer2 see ", val)
	}
}

func multi_consumer() {
	c := make(chan int)
	go consumer1(c)
	go consumer2(c)

	for i := 0; i < 10; i++ {
		c <- i
	}
}

func main() {
	// test_channel()
	multi_consumer()
}
