package main

import "fmt"

func fibonacci() func() int {
	g_p, p, fib := 0, 0, 0
	idx := 0
	return func() int {
		if idx == 0 {
			idx++
			return 0
		} else if idx == 1 {
			p = 1
			idx++
			return 1
		}

		fib = g_p + p
		g_p = p
		p = fib
		return fib
	}
}

func main() {
	f := fibonacci()
	for i := 0; i < 10; i++ {
		fmt.Println(f())
	}
}
