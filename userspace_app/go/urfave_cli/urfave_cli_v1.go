// https://github.com/urfave/cli/blob/master/docs/v1/manual.md
package main

import (
	"fmt"
	"log"
	"os"

	"github.com/urfave/cli"
)

var usage = fmt.Sprintf(`%s runtime

%s is a command line program for running applications packaged
according to the Open Container Initiative (OCI).`, "cli", "cli")

var cliCommands = []cli.Command{
	{
		Name:  "hello",
		Usage: "Say Hello",
		Flags: []cli.Flag{
			cli.StringFlag{
				Name:  "lang",
				Value: "english",
				Usage: "language for greeting",
			},
		},
		Action: func(c *cli.Context) error {
			name := "Nefertiti"
			if c.String("lang") == "spanish" {
				fmt.Println("Hola", name)
			} else {
				fmt.Println("Hello", name)
			}

			return nil
		},
	},
}

func main() {
	app := cli.NewApp()

	app.Name = "cli test"
	app.Usage = usage
	app.Commands = cliCommands
	app.EnableBashCompletion = true

	err := app.Run(os.Args)
	if err != nil {
		log.Fatal(err)
	}
}
