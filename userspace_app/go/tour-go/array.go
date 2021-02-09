package main

import "fmt"

type Caps [3]string

func (c Caps) show() {
	for _, cap := range c {
		fmt.Println(cap)
	}
}

func methodOnArray() {
	var c = [3]string{"eat", "fly", "hide"}
	Caps(c).show()
}

func test_on_array() {
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

	methodOnArray()
}

func test_two_dim_array() {
	n := 2
	m := 3
	//动态创建二维数组
	grid := make([][]int, n)
	for i := 0; i < n; i++ {
		grid[i] = make([]int, m)

		for j := 0; j < m; j++ {
			grid[i][j] = i*m + j
		}
	}
	/*
		012
		345
	*/
	for i := 0; i < n; i++ {
		for j := 0; j < m; j++ {
			fmt.Print(grid[i][j])
		}
		fmt.Println()
	}
}

func main() {
	// test_on_array()
	test_two_dim_array()
}
