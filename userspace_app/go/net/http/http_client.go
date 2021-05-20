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

func HttpRequest(param *ReqParam) (resp *http.Response, err error) {
	c := New()
	resp, err = c.Request(param.Method, param.Url, param.Body, param.Heads, param.Query)
	if err != nil {
		fmt.Println(err.Error())
		return
	}
	b, _ := httputil.DumpResponse(resp, true)
	fmt.Printf("DumpResponse: %s\n", string(b))
	return
}

type ReqParam struct {
	Method string
	Url    string
	Body   interface{}
	Heads  map[string]string
	Query  map[string]string
}

type ExpResp struct {
	StatusCode int
	Type       interface{}
}

type TestEntry struct {
	Param ReqParam
	Resp  ExpResp
}

type PingReq struct {
	Name string
	Val  string
}

type PingResp struct {
	Message string `json:"message"`
	Type    string `json:"type"`
	Marker  string `json:"marker"`
	Limits  string `json:"limits"`
	Name    string `json:"Name"`
}

var Tests = []TestEntry{
	TestEntry{
		Param: ReqParam{
			Method: http.MethodGet,
			Url:    HOST + "/ping",
			Body: PingReq{
				Name: "ceshi",
				Val:  "good",
			},
			Heads: map[string]string{
				"User": "weiyang",
			},
			Query: map[string]string{
				"marker": "mk",
				"limits": "10",
			},
		},
		Resp: ExpResp{
			StatusCode: 200,
			Type:       PingResp{},
		},
	},
}

func main() {

	for _, test := range Tests {
		resp, err := HttpRequest(&test.Param)
		if err != nil {
			fmt.Println("Response Error!")
			continue
		}
		if resp.StatusCode != test.Resp.StatusCode {
			fmt.Printf("Expect StatusCode %d but get %d\n",
				test.Resp.StatusCode, resp.StatusCode)
			continue
		}

		_, ok := test.Resp.Type.(PingResp)
		if ok {
			fmt.Println("Correct type")
		} else {
			fmt.Println("Incorrect type")
		}
	}
}
