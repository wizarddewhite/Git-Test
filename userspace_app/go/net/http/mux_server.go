package main

import (
	"github.com/gorilla/mux"
	"log"
	"net/http"
)

func YourHandler(w http.ResponseWriter, r *http.Request) {
	w.Write([]byte("Gorilla!\n"))
}

func handlePost(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(200)
	w.Write([]byte("good"))
}

func handleFunc() {
	r := mux.NewRouter()
	// Routes consist of a path and a handler function.
	r.HandleFunc("/", YourHandler)
	r.HandleFunc("/users", YourHandler).Methods("GET")
	r.HandleFunc("/user", handlePost).Methods("POST")

	// Bind to a port and pass our router in
	log.Fatal(http.ListenAndServe(":8000", r))
}

func handler() {
	router := mux.NewRouter().StrictSlash(true)
	router.Methods("GET").Path("/").Name("root").Handler(http.HandlerFunc(YourHandler))
	router.Methods("GET").Path("/user").Name("root").Handler(http.HandlerFunc(YourHandler))
	router.Methods("POST").Path("/token").Name("token").Handler(http.HandlerFunc(YourHandler))

	log.Fatal(http.ListenAndServe(":8000", router))
}

func main() {
	// handleFunc()
	handler()
}
