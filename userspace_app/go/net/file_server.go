package main

import (
	"fmt"
	"net/http"
	"os"
)

func main() {
	// use current dir as the root in web
	fileServer := http.FileServer(http.Dir("."))
	http.Handle("/", fileServer)

	//use /tmp as /tmpfile/ in web
	fileServer = http.FileServer(http.Dir("/tmp/"))
	http.Handle("/tmpfile/", http.StripPrefix("/tmpfile/", fileServer))

	// register the handler and deliver requests to it
	err := http.ListenAndServe(":8000", nil)
	checkError(err)
	// That's it!
}

func checkError(err error) {
	if err != nil {
		fmt.Println("Fatal error ", err.Error())
		os.Exit(1)
	}
}
