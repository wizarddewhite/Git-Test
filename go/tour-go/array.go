package main

import "fmt"

func main() {
	var a [2]string
	a[0] = "Hello"
	a[1] = "World"
	fmt.Println(a[0], a[1])
	fmt.Println(a)
	for i := 0; i < len(a); i++ {
		fmt.Printf("a[%d] == %s\n", i, a[i])
	}

	var b = [2]string{"a", "b"}
	fmt.Println(b)
	b[0] = "test"
	fmt.Println(b)

	var c = []string{"a", "b"}
	fmt.Println(c)
	c[0] = "test"
	fmt.Println(c)

}
