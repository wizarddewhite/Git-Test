package main

import "code.google.com/p/go-tour/reader"

type MyReader struct{}

// TODO: Add a Read(byte) (int, error) method to MyReader.
func (x MyReader) Read(b []byte) (int, error) {
	b[0] = 'A'
	return 1, nill
}

func main() {
	reader.Validate(MyReader{})
}

