package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
)

func direct_post() {
	data := make(map[string]interface{})
	data["name"] = "zhaofan"
	data["age"] = "23"
	bytesData, _ := json.Marshal(data)
	resp, _ := http.Post("http://localhost:8000/api/service?ide=vim", "application/json", bytes.NewReader(bytesData))
	body, _ := ioutil.ReadAll(resp.Body)
	fmt.Println(string(body))
}

func post_request() {
	client := &http.Client{}
	data := make(map[string]interface{})
	data["name"] = "zhaofan"
	data["age"] = "23"
	bytesData, _ := json.Marshal(data)
	req, _ := http.NewRequest("POST", "http://localhost:8000/api/service?ide=vim", bytes.NewReader(bytesData))
	req.Header.Add("User-Agent", "myClient")
	resp, _ := client.Do(req)
	defer resp.Body.Close()
}

type Hobby struct {
	Type string
	Name string
}

type User struct {
	Name  string `json:"name,omitempty"`
	Age   uint
	Hobby []Hobby
}

// https://kevin.burke.dev/kevin/golang-json-http/
func post_struct() {
	client := &http.Client{}
	data := User{
		Name: "zhang",
		Age:  12,
		Hobby: []Hobby{
			{
				Type: "sport",
				Name: "football",
			},
		},
	}
	bytesData, _ := json.Marshal(data)
	req, _ := http.NewRequest("POST", "http://localhost:8000/api/service?ide=vim", bytes.NewReader(bytesData))
	req.Header.Add("User-Agent", "myClient")
	resp, _ := client.Do(req)
	defer resp.Body.Close()
}

func main() {
	// direct_post()
	// post_request()
	post_struct()
}
