package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"net/http"
	"net/http/httputil"
)

const (
	VM_TOKEN = "503e3990c8fe3a3a7be079b360643612"
	HOST     = "http://localhost:8000"
)

type Client struct {
	client *http.Client
}

func New() *Client {
	return &Client{
		client: http.DefaultClient,
	}
}

func (c *Client) Request(method, url string, body interface{}, heads map[string]string) (*http.Response, error) {
	b, err := json.Marshal(body)
	if err != nil {
		return nil, err
	}

	req, err := http.NewRequest(method, url, bytes.NewReader(b))
	if err != nil {
		return nil, err
	}

	for k, v := range heads {
		req.Header.Add(k, v)
		fmt.Println("Adding head...")
	}
	return c.client.Do(req)
}

func HttpRequest(test *TestEntry) {
	c := New()
	resp, err := c.Request(test.Method, test.Url, test.Body, nil)
	if err != nil {
		fmt.Println(err.Error())
		return
	}
	b, _ := httputil.DumpResponse(resp, true)
	fmt.Printf("DumpResponse: %s\n", string(b))
}

type TestEntry struct {
	Method string
	Url    string
	Body   interface{}
}

var Tests = []TestEntry{
	TestEntry{
		Method: http.MethodGet,
		Url:    HOST + "/ping",
		Body:   nil,
	},
}

func main() {

	for _, test := range Tests {
		HttpRequest(&test)
	}
}
