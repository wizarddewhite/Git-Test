package main

import (
	"fmt"
	"os"

	"github.com/spf13/cobra"
)

var (
	nType   string
	dynamic bool
)

func newCommand() *cobra.Command {
	cmd := &cobra.Command{
		Use:   "cobra_cmd",
		Short: "cobra command",
		Long:  "cobra command for test",
		Run: func(c *cobra.Command, _ []string) {
			fmt.Println("command run")
		},
	}

	flags := cmd.Flags()
	flags.StringVar(&nType, "ntype", "", "What type of network to run")
	cmd.MarkFlagRequired("ntype")

	flags.BoolVar(&dynamic, "dynamic", false, "Dynamic config")

	return cmd
}

func main() {
	cmd := newCommand()

	if err := cmd.Execute(); err != nil {
		os.Exit(1)
	}
}
