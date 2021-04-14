package main

import (
	"fmt"
	"os"
	"path/filepath"

	"github.com/fsnotify/fsnotify"
)

//
var watcher *fsnotify.Watcher

// main
func main() {

	filepath.Walk(".", listDir)

	// creates a new file watcher
	watcher, _ = fsnotify.NewWatcher()
	defer watcher.Close()

	// starting at the root of the project, walk each file/directory searching for
	// directories
	if err := filepath.Walk(".", watchDir); err != nil {
		fmt.Println("ERROR", err)
	}

	//
	done := make(chan bool)

	//
	go func() {
		for {
			select {
			// watch for events
			case event := <-watcher.Events:
				if event.Op&fsnotify.Create == fsnotify.Create {
					fmt.Println("create file:", event.Name)
					filepath.Walk(event.Name, watchDir)
				} else if event.Op&fsnotify.Remove == fsnotify.Remove {
					fmt.Println("remove file:", event.Name)
					filepath.Walk(event.Name, watchDir)
				} else if event.Op&fsnotify.Write == fsnotify.Write {
					fmt.Println("modified file:", event.Name)
				} else {
					fmt.Printf("file: %s, event: %s\n", event.Name, event.Op.String())
				}
			// watch for errors
			case err := <-watcher.Errors:
				fmt.Println("ERROR", err)
			}
		}
	}()

	<-done
}

// watchDir gets run as a walk func, searching for directories to add watchers to
func watchDir(path string, fi os.FileInfo, err error) error {

	if fi == nil {
		fmt.Println("nil fi")
		return nil
	}

	// since fsnotify can watch all the files in a directory, watchers only need
	// to be added to each nested directory
	if fi.Mode().IsDir() {
		fmt.Println("Add dir: ", path)
		return watcher.Add(path)
	}

	return nil
}

func listDir(path string, fi os.FileInfo, err error) error {
	if fi.Mode().IsDir() && path != "/run/vm/" {
		fmt.Println(path, fi.Name(), fi.Size())
	}
	return nil
}
