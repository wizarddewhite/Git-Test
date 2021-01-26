package hello

import "rsc.io/quote"
import "example.com/hello/human"

func Hello() string {
	return quote.Hello()
}

func HelloHuman() string {
	return "Hello, " + human.Human()
}
