package main

import (
	"fmt"
	"path/filepath"
)

func main() {
	paths := []string{
		"/home/arnie/amelia.jpg",
		"/mnt/photos/",
		"rabbit.jpg",
		"/usr/local//go",
	}
	for _, p := range paths {
		dir := filepath.Dir(p)
		parent := filepath.Base(dir)
		_, file := filepath.Split(p)
		fmt.Printf("input: %q\n\tdir: %q\n\tparent: %q\n\tfile: %q\n", p, dir, parent, file)
	}
}
