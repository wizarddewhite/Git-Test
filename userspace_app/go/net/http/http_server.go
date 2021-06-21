package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
	"reflect"
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

type Request struct {
	Method  string
	URL     string
	Headers map[string][]string
	Queries map[string][]string
	Body    string
}

var idx int
var req = Request{
	Headers: make(map[string][]string),
	Queries: make(map[string][]string),
}

func saveHeaders(r *http.Request) {
	// clear original Header
	for name := range req.Headers {
		delete(req.Headers, name)
	}

	// save header to req
	for k, v := range r.Header {
		req.Headers[k] = v
	}
}

func saveQueries(r *http.Request) {
	// clear original Header
	for name := range req.Queries {
		delete(req.Queries, name)
	}

	// save header to req
	for k, v := range r.URL.Query() {
		req.Queries[k] = v
	}
}

func saveBody(r *http.Request) {
	defer r.Body.Close()
	body, _ := ioutil.ReadAll(r.Body)
	req.Body = string(body)
}

func sameHQ(old, new map[string][]string) bool {
	fmt.Println(old)
	fmt.Println(new)

	if reflect.DeepEqual(old, new) {
		return true
	}
	return false
}

func sameBody(old, new string) bool {
	fmt.Println(old)
	fmt.Println(new)

	var b1, b2 interface{}
	if err := json.Unmarshal([]byte(old), &b1); err != nil {
		return false
	}
	if err := json.Unmarshal([]byte(new), &b2); err != nil {
		return false
	}

	if reflect.DeepEqual(b1, b2) {
		return true
	}
	return false
}

func CompareRequest(w http.ResponseWriter, r *http.Request) {
	cur_idx := idx % 2
	fmt.Printf("req url: %s\n", r.URL.Path)

	if cur_idx == 0 {
		req.Method = r.Method
		req.URL = r.URL.Path
		saveHeaders(r)
		saveQueries(r)
		saveBody(r)
	} else {
		var field string
		var same bool
		var body []byte

		field = "method"
		if r.Method != req.Method {
			goto OUT
		}

		field = "url"
		if r.URL.Path != req.URL {
			goto OUT
		}

		field = "header"
		same = sameHQ(req.Headers, r.Header)
		if !same {
			goto OUT
		}

		field = "query"
		same = sameHQ(req.Queries, r.URL.Query())
		if !same {
			goto OUT
		}

		field = "body"
		defer r.Body.Close()
		body, _ = ioutil.ReadAll(r.Body)
		same = sameBody(req.Body, string(body))
		if !same {
			goto OUT
		} else {
			field = "request"
		}

	OUT:
		if same {
			fmt.Println("Same ", field)
		} else {
			fmt.Println("Different ", field)
		}
	}

	idx++
}

func main() {
	http.HandleFunc("/", IndexHandler)
	http.HandleFunc("/compare", CompareRequest)
	http.HandleFunc("/diffurl", CompareRequest)
	fmt.Println("Listen on :8000")
	http.ListenAndServe(":8000", nil)
}
