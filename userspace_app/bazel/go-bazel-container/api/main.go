package main

import (
	"encoding/json"
	"github.com/gorilla/mux"
	"log"
	"net/http"
)

func main() {
	router := mux.NewRouter().StrictSlash(true)
	router.HandleFunc("/hello/{name}", helloHandler).Methods("GET")

	log.Fatal(http.ListenAndServe(":8080", router))
}

type helloResponse struct {
	Message string
	Name string
}

func helloHandler(writer http.ResponseWriter, request *http.Request) {
	writer.Header().Set("Content-Type", "application/json")
	vars := mux.Vars(request)
	r := helloResponse{
		Message: "Hello",
		Name:    vars["name"],
	}

	encoder := json.NewEncoder(writer)
	_ = encoder.Encode(r)
}