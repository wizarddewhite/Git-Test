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

func normalize(s string) string {
	lower := strings.ToLower(s)
	noslash := strings.Replace(lower, "/", "-", -1)
	nounderscore := strings.Replace(noslash, "_", "-", -1)
	return nounderscore
}

func getVMName(user, service, group string) string {

	return fmt.Sprintf("u-%s.s-%s.g-%s", normalize(user), normalize(service), normalize(group))
}

func validateName(vmName, vmiName string) (row, col int) {
	row = -1
	col = -1
	// vmiName should start with vmName
	if !strings.HasPrefix(vmiName, vmName) {
		return
	}

	vmiIndex := strings.TrimPrefix(vmiName, vmName)
	s := strings.Split(vmiIndex, "-")
	if len(s) < 3 {
		return
	}
	row, err := strconv.Atoi(s[1])
	if err != nil {
		row = -1
		return
	}
	col, err = strconv.Atoi(s[2])
	if err != nil {
		row = -1
		col = -1
		return
	}
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

	fmt.Println("name: ", getVMName("tesla", "nevigator/road", "tour_SERVER"))

	row, col = validateName("example", "example-0-a")
	fmt.Println("index: ", row, col)
}
