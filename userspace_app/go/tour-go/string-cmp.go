package main

import "fmt"

func main() {
	str1 := "go"
	str2 := "goto"
	str3 := "ipvtap"
	str4 := "ipvlan"
	fmt.Println(str1 == str2)
	fmt.Println(str2[:2])
	fmt.Println(str1 == str2[:2])
	fmt.Println(str3[:3] == "ipv")
	fmt.Println(str4[:3] == "ipv")
}
