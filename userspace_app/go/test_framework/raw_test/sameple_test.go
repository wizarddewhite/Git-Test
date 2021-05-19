package main

import "testing"

func TestHello(t *testing.T) {
	got := Hello()
	want := "Hello, world"

	if got != want {
		t.Errorf("got %q want %q", got, want)
	}
}

func TestAdd(t *testing.T) {
	main()
	sum := Add(5, 5)
	if sum == 10 {
		t.Log("the result is ok")
	} else {
		t.Fatal("the result is wrong")
	}
}
