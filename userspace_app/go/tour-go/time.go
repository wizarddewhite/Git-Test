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

func time_format() {
	t := time.Now()
	// The Time type implements the Stringer interface -- it
	// has a String() method which gets called automatically by
	// functions like Printf().
	fmt.Println("raw:    ", t)
	// See the Constants section for more formats
	// http://golang.org/pkg/time/#Time.Format
	formatedTime := t.Format(time.RFC1123)
	fmt.Println("RFC1123:", formatedTime)
	t, err := time.Parse(time.RFC1123, formatedTime)
	if err != nil {
		fmt.Println(err)
	}
	fmt.Println("parse:  ", t.Format(time.RFC1123))

	formatedTime = t.Format("2006-01-02 15:04:05")
	fmt.Println("no-zone:", formatedTime)
	t, err = time.Parse("2006-01-02 15:04:05", formatedTime)
	if err != nil {
		fmt.Println(err)
	}
	fmt.Println("parse:  ", t)
}

func beginning_end_of_day() {
	t := time.Now()
	bt := time.Date(t.Year(), t.Month(), t.Day(), 0, 0, 0, 0, t.Location())
	fmt.Println(bt)
	et := time.Date(t.Year(), t.Month(), t.Day(), 23, 59, 59, 0, t.Location())
	fmt.Println(et)
}

func main() {

	// time_format()
	// ticker()
	beginning_end_of_day()
}
