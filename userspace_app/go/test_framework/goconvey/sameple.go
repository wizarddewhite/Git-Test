package main

import "fmt"

func CheckUrl(url string) bool {
	var urlList = [2]string{"learnku.com", "xdcute.com"}
	for v := range urlList {
		if urlList[v] == url {
			return true
		}
	}
	return false
}

func main() {
	fmt.Println("Hello")
}
