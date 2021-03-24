package main

import (
	"fmt"
	"log"
	"os"
	"time"
)

func main() {
	fileName := "temp.txt"
	_, err := os.Stat(fileName)
	if err == nil {
		// remove it first
		fmt.Println("remove ", fileName)
		os.Remove(fileName)
		// create it
		file, err := os.Create("temp.txt")
		if err != nil {
			log.Fatal(err)
		}
		defer file.Close()
	} else if os.IsNotExist(err) {
		file, err := os.Create("temp.txt")
		if err != nil {
			log.Fatal(err)
		}
		defer file.Close()
	}

	{
		currentTime := time.Now().Local()
		err = os.Chtimes(fileName, currentTime, currentTime)
		if err != nil {
			fmt.Println(err)
		}
	}
}
