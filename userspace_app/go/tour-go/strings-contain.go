package main

import (
	"fmt"
	"strings"
)

func main() {
	fmt.Println(strings.Contains("widuu", "wi")) //true
	fmt.Println(strings.Contains("wi", "widuu")) //false

	fmt.Println(strings.HasPrefix("vm.test.abd", "vm.test.")) //true
	fmt.Println(strings.HasPrefix("vm.test.abd", "vm.tesa"))  //false
}
