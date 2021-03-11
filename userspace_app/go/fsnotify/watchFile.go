package main

import (
	"fmt"

	"github.com/fsnotify/fsnotify"
)

// main
func main() {

	// creates a new file watcher
	watcher, err := fsnotify.NewWatcher()
	if err != nil {
		fmt.Println("ERROR", err)
	}
	defer watcher.Close()

	done := make(chan bool)

	go func() {
		for {
			select {
			// watch for events
			case event, ok := <-watcher.Events:
				if !ok {
					return
				}
				fmt.Println("EVENT! ", event)
				if event.Op&fsnotify.Write == fsnotify.Write {
					fmt.Println("modified file:", event.Name)
				}

			// watch for errors
			case err, ok := <-watcher.Errors:
				if !ok {
					return
				}
				fmt.Println("ERROR", err)
			}
		}
	}()

	// out of the box fsnotify can watch a single file, or a single directory
	if err := watcher.Add("./tmpfile"); err != nil {
		fmt.Println("ERROR", err)
	}

	<-done
}
