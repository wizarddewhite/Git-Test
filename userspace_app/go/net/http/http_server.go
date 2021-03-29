package main

import (
	"fmt"
	"io/ioutil"
	"net/http"
)

func IndexHandler(w http.ResponseWriter, r *http.Request) {
	fmt.Printf("req url: %s\n", r.URL.Path)
	fmt.Printf("raw query : %s\n", r.URL.RawQuery)
	vars := r.URL.Query()
	for k, v := range vars {
		fmt.Printf("\t%s = %s\n", k, v[0])
	}
	fmt.Printf("req headers : \n")
	for k, v := range r.Header {
		fmt.Printf("\t%s = %s\n", k, v[0])
	}
	defer r.Body.Close()
	body, _ := ioutil.ReadAll(r.Body)

	fmt.Printf("body: %v\n", string(body))

	fmt.Fprintln(w, "hello world")
}

func main() {
	http.HandleFunc("/", IndexHandler)
	fmt.Println("Listen on :8000")
	http.ListenAndServe(":8000", nil)
}
