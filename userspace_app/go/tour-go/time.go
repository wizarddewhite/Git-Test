package main

import (
	"fmt"
	"time"
)

func ticker() {
	ticker := time.NewTicker(1500 * time.Millisecond)
	done := make(chan bool)
	env := make(chan int)

	go func() {
		for {
			select {
			case val := <-env:
				fmt.Println("Receive env ", val)
			case <-done:
				fmt.Println("Receive done")
				return
			case t := <-ticker.C:
				fmt.Println("Tick at", t)
			}
		}
	}()

	time.Sleep(16000 * time.Millisecond)
	fmt.Println("Stop ticker")
	ticker.Stop()
	time.Sleep(4600 * time.Millisecond)
	env <- 1
	done <- true
	time.Sleep(2600 * time.Millisecond)
	fmt.Println("ticker return")
}

func main() {
	t := time.Now()
	// The Time type implements the Stringer interface -- it
	// has a String() method which gets called automatically by
	// functions like Printf().
	fmt.Printf("%s\n", t)
	// See the Constants section for more formats
	// http://golang.org/pkg/time/#Time.Format
	formatedTime := t.Format(time.RFC1123)
	fmt.Println(formatedTime)
	formatedTime = t.Format("2006-01-02 15:04:05")
	fmt.Println(formatedTime)

	t, err := time.Parse("2006-01-02 15:04:05", formatedTime)
	if err != nil {
		fmt.Println(err)
	}
	fmt.Println(t)

	// ticker()
}
