package main

import (
	"fmt"
	"log"
	"net/http"
)

type String string

func (s String) ServeHTTP (w http.ResponseWriter, r *http.Request) {
	fmt.Fprint(w, s)
}

type Struct struct {
    Greeting string
    Punct    string
    Who      string
}

func main() {
	// your http.Handle calls here
	var s String
	log.Fatal(http.ListenAndServe("localhost:4000", s))
}
