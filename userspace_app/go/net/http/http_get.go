package main

import (
	"fmt"
	"net/http"
	"net/http/httputil"
	"net/url"
	"os"
	"strings"
)

func acceptableCharset(contentTypes []string) bool {
	// each type is like [text/html; charset=UTF-8]
	// we want the UTF-8 only
	for _, cType := range contentTypes {
		if strings.Index(cType, "utf-8") != -1 {
			return true
		}
	}
	return false
}
func raw_get() {
	url := os.Args[1]

	response, err := http.Get(url)
	if err != nil {
		fmt.Println(err.Error())
		os.Exit(2)
	}

	b, _ := httputil.DumpResponse(response, true)
	fmt.Printf("DumpResponse: %s\n", string(b))

	contentTypes := response.Header["Content-Type"]
	if !acceptableCharset(contentTypes) {
		fmt.Println("Cannot handle", contentTypes)
		os.Exit(4)
	}

	// parse response body
	var buf [512]byte
	reader := response.Body
	fmt.Println("====\nResponse Body:\n====")
	for {
		n, err := reader.Read(buf[0:])
		if err != nil {
			os.Exit(0)
		}
		fmt.Println(string(buf[0:n]))
	}

	os.Exit(0)
}

func getCharset(response *http.Response) string {
	contentType := response.Header.Get("Content-Type")
	if contentType == "" {
		// guess
		return "UTF-8"
	}
	idx := strings.Index(contentType, "charset")
	if idx == -1 {
		// guess
		return "UTF-8"
	}
	strs := strings.Split(contentType[idx:], "=")
	if len(strs) == 2 {
		return strings.ToUpper(strs[1])
	}

	return "UTF-8"
}

func checkError(err error) {
	if err != nil {
		fmt.Println("Fatal error ", err.Error())
		os.Exit(1)
	}
}

func http_client() {
	url, err := url.Parse(os.Args[1])
	checkError(err)

	client := &http.Client{}
	request, err := http.NewRequest("GET", url.String(), nil)
	checkError(err)
	request.Header.Add("Accept-Charset", "UTF-8;q=1, ISO-8859-1;q=0")

	// send http request and get response
	response, err := client.Do(request)
	if err != nil {
		fmt.Println("Error: ", err.Error())
		os.Exit(2)
	}
	if response.Status != "200 OK" {
		fmt.Println(response.Status)
		os.Exit(2)
	}

	fmt.Println("====\nResponse Head:\n====")
	b, _ := httputil.DumpResponse(response, true)
	fmt.Print(string(b))
	chSet := getCharset(response)
	fmt.Printf("got charset: %s\n", chSet)
	if chSet != "UTF-8" {
		fmt.Println("Cannot handle", chSet)
		os.Exit(4)
	}

	var buf [512]byte
	reader := response.Body
	fmt.Println("====\nResponse Body:\n====")
	for {
		n, err := reader.Read(buf[0:])
		if err != nil {
			os.Exit(0)
		}
		fmt.Print(string(buf[0:n]))
	}

	os.Exit(0)
}

func main() {
	if len(os.Args) != 2 {
		fmt.Println("Usage: ", os.Args[0], "http://host:port[/page]")
		os.Exit(1)
	}

	// raw_get()
	http_client()
}