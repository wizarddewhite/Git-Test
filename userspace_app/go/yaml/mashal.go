// https://github.com/ghodss/yaml
package main

import (
	"fmt"
	"io/ioutil"

	"github.com/ghodss/yaml"
)

type Person struct {
	Name string `json:"name"` // Affects YAML field names too.
	Age  int    `json:"age"`
}

func in_mem() {
	// Marshal a Person struct to YAML.
	p := Person{"John", 30}
	y, err := yaml.Marshal(p)
	if err != nil {
		fmt.Printf("err: %v\n", err)
		return
	}
	fmt.Println(string(y))
	/* Output:
	age: 30
	name: John
	*/

	// Unmarshal the YAML back into a Person struct.
	var p2 Person
	err = yaml.Unmarshal(y, &p2)
	if err != nil {
		fmt.Printf("err: %v\n", err)
		return
	}
	fmt.Println(p2)
	/* Output:
	{John 30}
	*/
}

func from_file() {
	dat, _ := ioutil.ReadFile("person.yaml")
	fmt.Print(string(dat))
	fmt.Printf("Type is %T\n", dat)

	var per Person
	err := yaml.Unmarshal(dat, &per)
	if err != nil {
		fmt.Printf("err: %v\n", err)
		return
	}
	fmt.Println(per)
}

func main() {
	// in_mem()
	from_file()
}
