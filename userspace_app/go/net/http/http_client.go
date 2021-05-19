package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"net/http"
	"net/http/httputil"
	"strings"
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

func (c *Client) Request(method, url string, body interface{}, heads, query map[string]string) (*http.Response, error) {
	b, err := json.Marshal(body)
	if err != nil {
		return nil, err
	}

	var query_str string
	if len(query) != 0 {
		var qs []string
		for k, v := range query {
			qs = append(qs, fmt.Sprintf("%s=%s", k, v))
		}
		query_str = "?" + strings.Join(qs, "&")
	}

	req, err := http.NewRequest(method, url+query_str, bytes.NewReader(b))
	if err != nil {
		return nil, err
	}

	for k, v := range heads {
		req.Header.Add(k, v)
	}
	return c.client.Do(req)
}

func HttpRequest(test *TestEntry) {
	c := New()
	resp, err := c.Request(test.Method, test.Url, test.Body, test.Heads, test.Query)
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
	Heads  map[string]string
	Query  map[string]string
}

var Tests = []TestEntry{
	TestEntry{
		Method: http.MethodGet,
		Url:    HOST + "/ping",
		Heads: map[string]string{
			"User": "weiyang",
		},
		Query: map[string]string{
			"marker": "mk",
			"limits": "10",
		},
	},
}

func main() {

	for _, test := range Tests {
		HttpRequest(&test)
	}
}
