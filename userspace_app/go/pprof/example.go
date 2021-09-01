// https://segmentfault.com/a/1190000016412013
// Usage:
// 1. http://127.0.0.1:8080/debug/pprof/
// 2. go tool pprof http://localhost:8080/debug/pprof/heap

package main

import (
	"log"
	"net/http"
	_ "net/http/pprof"
	// "time"
)

var datas []string

func Add(str string) string {
	data := []byte(str)
	sData := string(data)
	datas = append(datas, sData)

	return sData
}

func main() {
	go func() {
		for {
			// time.Sleep(time.Second)
			log.Println(Add("https://github.com/EDDYCJY"), len(datas))
		}
	}()

	http.ListenAndServe("0.0.0.0:8080", nil)
}
