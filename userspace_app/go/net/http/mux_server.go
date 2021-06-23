package main

import (
	"github.com/gorilla/mux"
	"log"
	"net/http"
)

func YourHandler(w http.ResponseWriter, r *http.Request) {
	w.Write([]byte("Gorilla!\n"))
}

func handleFunc() {
	r := mux.NewRouter()
	// Routes consist of a path and a handler function.
	r.HandleFunc("/", YourHandler)
	r.HandleFunc("/user", YourHandler).Methods("GET")

	// Bind to a port and pass our router in
	log.Fatal(http.ListenAndServe(":8000", r))
}

func handler() {
	router := mux.NewRouter().StrictSlash(true)
	router.Methods("GET").Path("/").Name("root").Handler(http.HandlerFunc(YourHandler))
	router.Methods("GET").Path("/user").Name("root").Handler(http.HandlerFunc(YourHandler))

	log.Fatal(http.ListenAndServe(":8000", router))
}

func main() {
	//handleFunc()
	handler()
}
