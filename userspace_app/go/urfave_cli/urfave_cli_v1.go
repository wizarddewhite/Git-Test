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

var networkCLICommand = cli.Command{
	Name:  "network",
	Usage: "manage interfaces and routes for container",
	Action: func(c *cli.Context) error {
		fmt.Println("Adjust network interface")
		return nil
	},
}

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
			fmt.Println(c.GlobalString("country"))
			name := "Nefertiti"
			if c.String("lang") == "spanish" {
				fmt.Println("Hola", name)
			} else {
				fmt.Println("Hello", name)
			}

			return nil
		},
	},
	networkCLICommand,
}

var cliBeforeSubcommands = beforeSubcommands

func beforeSubcommands(c *cli.Context) error {
	fmt.Println("###Before Subcommand")
	return nil
}

var cliAfterSubcommands = afterSubcommands

func afterSubcommands(c *cli.Context) error {
	fmt.Println("---After Subcommand")
	return nil
}

var cliCommandNotFound = commandNotFound

func commandNotFound(c *cli.Context, command string) {
	fmt.Printf("Invalid command %q \n", command)
	os.Exit(1)
}

func main() {
	app := cli.NewApp()

	app.Name = "cli test"
	app.Usage = usage
	app.Commands = cliCommands
	app.EnableBashCompletion = true
	app.Before = cliBeforeSubcommands
	app.After = cliAfterSubcommands
	app.CommandNotFound = cliCommandNotFound

	app.Flags = []cli.Flag{
		&cli.StringFlag{
			Name:  "country",
			Value: "chn",
			Usage: "country",
		},
	}

	err := app.Run(os.Args)
	if err != nil {
		log.Fatal(err)
	}
}
