package main

import (
	"fmt"
	"strconv"
	"strings"
)

func convertName(name string) string {
	s := strings.Split(name, "-")
	return strings.Join(s[:len(s)-3], "-")
}

func getIndex(name string) (row, col int) {
	s := strings.Split(name, "-")
	length := len(s)
	row, _ = strconv.Atoi(s[length-3])
	col, _ = strconv.Atoi(s[length-2])
	return
}

func main() {
	fmt.Printf("%q\n", strings.Split("a,b,c", ","))
	fmt.Printf("%q\n", strings.Split("example-vm-0-1-zjlfp", "-"))

	fmt.Printf("%q\n", convertName("example-vm-0-1-zjlfp"))
	row, col := getIndex("example-vm-0-1-zjlfp")
	fmt.Printf("row:%d col:%d\n", row, col)
	row, col = getIndex("example-vm-0-10-zjlfp")
	fmt.Printf("row:%d col:%d\n", row, col)
}
