package main

import (
	"fmt"
	"time"
)

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

	ticker := time.NewTicker(500 * time.Millisecond)
	done := make(chan bool)

	go func() {
		for {
			select {
			case <-done:
				return
			case t := <-ticker.C:
				fmt.Println("Tick at", t)
			}
		}
	}()

	time.Sleep(2600 * time.Millisecond)
	ticker.Stop()
	done <- true
	fmt.Println("Ticker stopped")
}
