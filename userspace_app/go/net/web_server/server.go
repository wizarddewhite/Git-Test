package main

import (
	"fmt"
	"io/ioutil"
	"net/http"
	"os"
	"regexp"
	"text/template"
)

import (
	"flashcards"
	"github.com/wizarddewhite/Go_example/dictionary"
	"templatefuncs"
)

var d *dictionary.Dictionary

func main() {
	if len(os.Args) != 2 {
		fmt.Fprint(os.Stderr, "Usage: ", os.Args[0], ":port\n")
		os.Exit(1)
	}
	port := os.Args[1]

	dictionaryPath := "cedict_ts.u8"
	d = new(dictionary.Dictionary)
	d.Load(dictionaryPath)
	fmt.Println("Loaded dict", len(d.Entries))

	//http.HandleFunc("/", listFlashCards)
	//fileServer := http.StripPrefix("/jscript/", http.FileServer(http.Dir("jscript")))
	//http.Handle("/jscript/", fileServer)
	//fileServer = http.StripPrefix("/html/", http.FileServer(http.Dir("html")))
	//http.Handle("/html/", fileServer)

	//http.HandleFunc("/wordlook", lookupWord)
	//http.HandleFunc("/flashcards.html", listFlashCards)
	//http.HandleFunc("/flashcardSets", manageFlashCards)
	//http.HandleFunc("/searchWord", searchWord)
	//http.HandleFunc("/addWord", addWord)
	//http.HandleFunc("/newFlashCardSet", newFlashCardSet)

	//err := http.ListenAndServe(port, nil)
	//checkError(err)
	// That's it!
}

func checkError(err error) {
	if err != nil {
		fmt.Println("Fatal error ", err.Error())
		os.Exit(1)
	}
}
