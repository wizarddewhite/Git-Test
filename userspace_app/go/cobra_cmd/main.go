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

var versionCmd = &cobra.Command{
	Use:   "version",
	Short: "Print the version number of Hugo",
	Long:  `All software has versions. This is Hugo's`,
	Args:  cobra.NoArgs,
	Run: func(cmd *cobra.Command, args []string) {
		fmt.Println("Hugo Static Site Generator v0.9 -- HEAD")
	},
}

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

	cmd.AddCommand(versionCmd)

	return cmd
}

func main() {
	cmd := newCommand()

	if err := cmd.Execute(); err != nil {
		os.Exit(1)
	}
}
