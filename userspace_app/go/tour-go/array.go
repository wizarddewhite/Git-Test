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

func dump_two_dim_array(array [][]int) {
	row := len(array)
	column := len(array[0])

	fmt.Printf("array: %p, row: %d, column: %d\n", &array, row, column)
	for i := 0; i < row; i++ {
		fmt.Print("\t")
		for j := 0; j < column; j++ {
			fmt.Print(array[i][j])
		}
		fmt.Println()
	}
}

func adjust_two_dim_array(array [][]int, row, column int) [][]int {
	curr_row := len(array)
	curr_column := len(array[0])
	fmt.Printf("Adjust array from %dx%d to %dx%d\n",
		curr_row, curr_column, row, column)

	for i := 0; i < curr_row; i++ {
		// adjust column
		if column > curr_column {
			// grow column
			array[i] = append(array[i], make([]int, column-curr_column)...)
		} else if column < curr_column {
			// shrink column
			array[i] = array[i][:column]
		}
	}

	if row > curr_row {
		// grow row
		array = append(array, make([][]int, row-curr_row)...)
		// allocate column
		for i := curr_row; i < row; i++ {
			array[i] = make([]int, column)
		}
	} else if row < curr_row {
		// shrink row
		array = array[:row]
	}

	dump_two_dim_array(array)
	return array
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
	dump_two_dim_array(grid)

	grid = adjust_two_dim_array(grid, 2, 4)
	grid = adjust_two_dim_array(grid, 2, 2)
	grid = adjust_two_dim_array(grid, 1, 2)

	// dump_two_dim_array(grid)
	adjust_two_dim_array(grid, 3, 2)
}

func iter_two_dim_array(array [][]int, row, column int) {
	curr_row := len(array)
	curr_column := len(array[0])

	if row >= curr_row || column >= curr_column {
		fmt.Printf("no more element after %d %d\n", row, column)
		return
	} else {
		fmt.Printf("element after %d %d\n", row, column)
	}

	for i := row; i < curr_row; i++ {
		for j := column; j < curr_column; j++ {
			fmt.Printf(" %d", array[i][j])
		}
		column = 0
	}
	fmt.Println("")
}

func test_two_dim_array_iter() {
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
	dump_two_dim_array(grid)
	iter_two_dim_array(grid, 2, 3)
	iter_two_dim_array(grid, 2, 0)
	iter_two_dim_array(grid, 1, 0)
	iter_two_dim_array(grid, 0, 1)
}

func main() {
	// test_on_array()
	// test_two_dim_array()
	test_two_dim_array_iter()
}
