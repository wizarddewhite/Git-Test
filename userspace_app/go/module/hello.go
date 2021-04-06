package hello

import (
	"example.com/hello/animal"
	"example.com/hello/human"
	"rsc.io/quote"
)

func Hello() string {
	return quote.Hello()
}

func HelloHuman() string {
	return "Hello, " + human.Human()
}

func HelloAnimal() string {
	return "Hello, " + animal.Animal()
}
