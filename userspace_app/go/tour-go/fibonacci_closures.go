package main

import "fmt"

func fibonacci() func() int {
	/* use -1 to indicate the value is uninitialized */
	gp, p, fib := -1, -1, 0
	return func() int {
		if p == -1 {
			p = 0
			return 0
		} else if gp == -1 {
			p = 1
			gp = 0
			return 1
		}

		fib = gp + p
		gp = p
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
