package main

import "fmt"

func create_slice() {
	/* Format 1 */
	var p = []int{2, 3, 5, 7, 11, 13}
	fmt.Println("p ==", p)
	for i := 0; i < len(p); i++ {
		fmt.Printf("p[%d] == %d\n", i, p[i])
	}
	printSlice("p", p)

	/* Format 2 */
	var a []int
	printSlice("a", a)
	if a == nil {
		fmt.Println("nil!")
	}
	a = make([]int, 5, 5)
	printSlice("a", a)
	a[0] = 1

	/* Format 3 */
	b := make([]int, 0, 5)
	printSlice("b", b)
}

func make_slice() {
	b := make([]int, 0, 5)
	printSlice("b", b)

	/* Slice of the first two */
	c := b[:2]
	printSlice("c", c)
	fmt.Println(&c)

	/* Slice of the last three */
	d := c[2:5]
	printSlice("d", d)
	fmt.Println(&d)
}

func append_slice() {
	var a []int
	printSlice("a", a)

	/* Append one element */
	a = append(a, 0)
	printSlice("a", a)

	/* Append several elements */
	a = append(a, 1, 2, 3, 4)
	printSlice("a", a)

	/* Copy a to b */
	b := make([]int, len(a), cap(a))
	copy(b, a)
	printSlice("b", b)
}

func main() {
	//create_slice()
	make_slice()
	//append_slice()
}

func printSlice(s string, x []int) {
	fmt.Printf("%s len=%d cap=%d %v\n",
		s, len(x), cap(x), x)
}