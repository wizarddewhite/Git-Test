// https://segmentfault.com/a/1190000016412013
// Usage:
// 1. http://127.0.0.1:8080/debug/pprof/
// 2. go tool pprof http://localhost:8080/debug/pprof/heap
//      or
//    curl -sK -v http://localhost:8080/debug/pprof/heap > heap.out
//    go tool pprof heap.out
// 3. go tool pprof heap.out
//    (pprof) png
//    (pprof) web

// Official doc:
//   https://golang.org/doc/diagnostics

// MemoryLeak Example
//   https://blog.detectify.com/2019/09/05/how-we-tracked-down-a-memory-leak-in-one-of-our-go-microservices/
// Possible cause for memory leak
//    Creating substrings and subslices.
//    Wrong use of the defer statement.
//    Unclosed HTTP response bodies (or unclosed resources in general).
//    Orphaned hanging go routines.
//    Global variables.

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
