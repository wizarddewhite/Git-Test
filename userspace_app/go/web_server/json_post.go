package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"os"
)

type Stat struct {
	Name string
	Used string
}

type StatSlice struct {
	Token string
	Stats []Stat
}

func main() {
	var s StatSlice
	s.Token = "123456"
	s.Stats = append(s.Stats, Stat{Name: "weiyang", Used: "123"})
	s.Stats = append(s.Stats, Stat{Name: "liyang", Used: "323"})
	b, err := json.Marshal(s)
	if err != nil {
		fmt.Println("json err:", err)
	}

	res, _ := http.Post("http://localhost:8080/statistic/update", "application/json; charset=utf-8", bytes.NewReader(b))
	io.Copy(os.Stdout, res.Body)
	//var result Res
	//json.NewDecoder(res.Body).Decode(&result)
	//fmt.Println(result.status)
}
