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

func HttpGetWithTimeOut(api string, header map[string]string, timeout int) bool {
	return true
}

func main() {
	fmt.Println("Hello")
}
