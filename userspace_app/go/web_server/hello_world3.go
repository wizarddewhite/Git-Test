package main

import (
	"fmt"
	"io"
	"net/http"
)

type apiHandler struct{}

func (apiHandler) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	io.WriteString(w, "Hello world!")
}

func main() {
	mux := http.NewServeMux()
	mux.Handle("/hello_world/", apiHandler{})
	mux.HandleFunc("/", func(w http.ResponseWriter, req *http.Request) {
		// The "/" pattern matches everything, so we need to check
		// that we're at the root here.
		if req.URL.Path != "/" {
			io.WriteString(w, "My server: "+req.URL.String())
			return
		}
		fmt.Fprintf(w, "Welcome to the home page!")
	})

	server := http.Server{
		Addr:    ":8000",
		Handler: mux,
	}
	server.ListenAndServe()
}
